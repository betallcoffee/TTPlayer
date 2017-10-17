//
//  TTAudioQueue_ios.cpp
//  TTPlayerExample
//
//  Created by liang on 10/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTAudioQueue_ios.hpp"

using namespace TT;

void CheckError(OSStatus error, const char *operation) {
    if (error == noErr) return;
    char errorString[20];
    // See if it appears to be a 4-char-code
    *(uint32_t *)(errorString) = CFSwapInt32HostToBig(error);
    if (isprint(errorString[0]) && isprint(errorString[1]) &&
        isprint(errorString[2]) && isprint(errorString[3])) {
        errorString[4] = '\0';
    } else {
        // No, format it as an integer
        sprintf(errorString, "%d", (int)error);
    }
    fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
}

AudioStreamBasicDescription CreateASBDWithSampleRate(float sampleRate,bool isFloat, int numberOfChannels, bool interleavedIfStereo) {
    AudioStreamBasicDescription asbd = {0};
    int sampleSize          = isFloat ? sizeof(float) : sizeof(SInt16);
    asbd.mChannelsPerFrame  = (numberOfChannels == 1) ? 1 : 2;
    asbd.mBitsPerChannel    = 8 * sampleSize;
    asbd.mFramesPerPacket   = 1;
    asbd.mSampleRate        = sampleRate;
    asbd.mBytesPerFrame     = interleavedIfStereo ? sampleSize * asbd.mChannelsPerFrame : sampleSize;
    asbd.mBytesPerPacket    = asbd.mBytesPerFrame;
    asbd.mReserved          = 0;
    asbd.mFormatID          = kAudioFormatLinearPCM;
    if (isFloat) {
        asbd.mFormatFlags = kAudioFormatFlagIsFloat;
        if (interleavedIfStereo) {
            if (numberOfChannels == 1) {
                asbd.mFormatFlags = asbd.mFormatFlags | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
            }
        }
        else{
            asbd.mFormatFlags = asbd.mFormatFlags | kAudioFormatFlagIsNonInterleaved | kAudioFormatFlagIsPacked ;
        }
    }
    else{
        asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
        if (!interleavedIfStereo) {
            if (numberOfChannels > 1) {
                asbd.mFormatFlags = asbd.mFormatFlags | kAudioFormatFlagIsNonInterleaved;
            }
        }
    }
    return asbd;
}

bool AudioQueue_ios::setup(AudioDesc &desc) {
    _audioFormat = CreateASBDWithSampleRate(desc.sampleRate, true, desc.channels, true);
    CheckError(AudioQueueNewOutput(&_audioFormat,
                                     AudioQueue_ios::queueCallback,
                                     this,
                                     NULL,  // run loop
                                     NULL,  // run loop mode
                                     0,     // flags
                                     &_playQueue), "AudioQueueNewOutput error");
    
    
    UInt32 sizeOfBuff = desc.nbSamples * _audioFormat.mBytesPerPacket;
    for (int i = 0; i < kNumberOfAudioDataBuffers; ++i) {
        CheckError(AudioQueueAllocateBuffer(_playQueue,
                                              sizeOfBuff,
                                              &_playQueueBuffers[i]), "AudioQueueAllocateBuffer error");
    }
    
    for (int i = 0; i < kNumberOfAudioDataBuffers; ++i) {
        queueCallback(this, _playQueue, _playQueueBuffers[i]);
    }
    
    start();
    
    return true;
}

void AudioQueue_ios::teardown() {
    stop();
    
    for (int t = 0; t < kNumberOfAudioDataBuffers; ++t) {
        CheckError(AudioQueueFreeBuffer(_playQueue, _playQueueBuffers[t]), "AudioQueueFreeBuffer error");
    }
    
    CheckError(AudioQueueDispose(_playQueue, true), "AudioQueueDispose error");
    
    _playQueue = nullptr;
}

void AudioQueue_ios::start() {
    CheckError(AudioQueueStart(_playQueue, NULL), "AudioQueueStart error");
}

void AudioQueue_ios::stop() {
    CheckError(AudioQueueStop(_playQueue, TRUE), "AudioQueueStop error");
}

void AudioQueue_ios::pause() {
    CheckError(AudioQueuePause(_playQueue), "AudioQueuePause error");
    CheckError(AudioQueueReset(_playQueue), "AudioQueueReset error");
}

void AudioQueue_ios::resume() {
    start();
}

void AudioQueue_ios::queueCallback(void *inUserData, AudioQueueRef inAudioQueue, AudioQueueBufferRef inBuffer) {
    if (inUserData == nullptr) {
        return;
    }
    
    AudioQueue_ios *self = (AudioQueue_ios *)inUserData;
    return self->_queueCallback(inUserData, inAudioQueue, inBuffer);
}

void AudioQueue_ios::_queueCallback(void *inUserData, AudioQueueRef inAudioQueue, AudioQueueBufferRef inBuffer) {
    AudioQueueSetParameter(inAudioQueue, kAudioQueueParam_Volume, 1);
    if (_cb) {
        std::shared_ptr<Frame> frame = _cb();
        if (frame && frame->data[0] && frame->lineSize[0] > 0) {
            memcpy(inBuffer->mAudioData, frame->data[0], frame->lineSize[0]);
            inBuffer->mAudioDataByteSize = frame->lineSize[0];
        } else if (inBuffer->mAudioData) {
            memset(inBuffer->mAudioData, 0, inBuffer->mAudioDataBytesCapacity);
            inBuffer->mAudioDataByteSize = inBuffer->mAudioDataBytesCapacity;
        }
        CheckError(AudioQueueEnqueueBuffer(inAudioQueue, inBuffer, 0, NULL), "AudioQueueEnqueueBuffer error");
    }
}
