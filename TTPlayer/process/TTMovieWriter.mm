//
//  TTMovieWriter.m
//  TTPlayerExample
//
//  Created by liang on 21/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

#include "easylogging++.h"
#include "TTFramebuffer.hpp"
#import "TTMovieWriter.h"

static const GLchar *const kColorSwizzlingFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 void main()
 {
     gl_FragColor = texture2D(texture, v_texcoord).bgra;
 }
 );

@interface TTMovieWriter ()
{
    NSURL *_movieURL;
    NSString *_fileType;
    CGSize _videoSize;
    
    NSMutableDictionary *_videoOutputSettings;
    NSMutableDictionary *_audioOutputSettings;
    
    CMTime _startTime, _previousFrameTime, _previousAudioTime;
    CMTime _offsetTime;
    
    AVAssetWriter *_assetWriter;
    AVAssetWriterInput *_assetWriterAudioInput;
    AVAssetWriterInput *_assetWriterVideoInput;
    AVAssetWriterInputPixelBufferAdaptor *_assetWriterPixelBufferInput;
    
    dispatch_queue_t _movieWriterQueue;
}

@end

@implementation TTMovieWriter

- (id)initWithMovieURL:(NSURL *)newMovieURL size:(CGSize)newSize {
    return [self initWithMovieURL:newMovieURL
                             size:newSize
                         fileType:AVFileTypeQuickTimeMovie
              videoOutputSettings:nil
              audioOutputSettings:nil];
}

- (instancetype)initWithMovieURL:(NSURL *)newMovieURL
                            size:(CGSize)newSize
                        fileType:(NSString *)newFileType
             videoOutputSettings:(NSMutableDictionary *)videoOutputSettings
             audioOutputSettings:(NSDictionary *)audioOutputSettings {
    self = [super init];
    if (self) {
        _movieURL = [newMovieURL copy];
        _fileType = newFileType;
        _videoSize = newSize;
        _startTime = kCMTimeInvalid;
        _previousFrameTime = kCMTimeNegativeInfinity;
        _previousAudioTime = kCMTimeNegativeInfinity;
        _videoOutputSettings = [videoOutputSettings copy];
        _audioOutputSettings = [audioOutputSettings copy];
        
        _movieWriterQueue = dispatch_queue_create("movie writer", DISPATCH_QUEUE_SERIAL);
    }
    return self;
}

- (void)setupVideoWriter {
    // use default output settings if none specified
    if (_videoOutputSettings == nil)
    {
        NSMutableDictionary *settings = [[NSMutableDictionary alloc] init];
        [settings setObject:AVVideoCodecH264 forKey:AVVideoCodecKey];
        [settings setObject:[NSNumber numberWithInt:_videoSize.width] forKey:AVVideoWidthKey];
        [settings setObject:[NSNumber numberWithInt:_videoSize.height] forKey:AVVideoHeightKey];
        _videoOutputSettings = settings;
    }
    
    /*
     NSDictionary *videoCleanApertureSettings = [NSDictionary dictionaryWithObjectsAndKeys:
     [NSNumber numberWithInt:videoSize.width], AVVideoCleanApertureWidthKey,
     [NSNumber numberWithInt:videoSize.height], AVVideoCleanApertureHeightKey,
     [NSNumber numberWithInt:0], AVVideoCleanApertureHorizontalOffsetKey,
     [NSNumber numberWithInt:0], AVVideoCleanApertureVerticalOffsetKey,
     nil];
     
     NSDictionary *videoAspectRatioSettings = [NSDictionary dictionaryWithObjectsAndKeys:
     [NSNumber numberWithInt:3], AVVideoPixelAspectRatioHorizontalSpacingKey,
     [NSNumber numberWithInt:3], AVVideoPixelAspectRatioVerticalSpacingKey,
     nil];
     
     NSMutableDictionary * compressionProperties = [[NSMutableDictionary alloc] init];
     [compressionProperties setObject:videoCleanApertureSettings forKey:AVVideoCleanApertureKey];
     [compressionProperties setObject:videoAspectRatioSettings forKey:AVVideoPixelAspectRatioKey];
     [compressionProperties setObject:[NSNumber numberWithInt: 2000000] forKey:AVVideoAverageBitRateKey];
     [compressionProperties setObject:[NSNumber numberWithInt: 16] forKey:AVVideoMaxKeyFrameIntervalKey];
     [compressionProperties setObject:AVVideoProfileLevelH264Main31 forKey:AVVideoProfileLevelKey];
     
     [outputSettings setObject:compressionProperties forKey:AVVideoCompressionPropertiesKey];
     */
    
    _assetWriterVideoInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo
                                                                outputSettings:_videoOutputSettings];
    
    // TODO live
    _assetWriterVideoInput.expectsMediaDataInRealTime = YES;
    
    // You need to use BGRA for the video in order to get realtime encoding. I use a color-swizzling shader to line up glReadPixels' normal RGBA output with the movie input's BGRA.
    NSDictionary *sourcePixelBufferAttributesDictionary = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
                                                           [NSNumber numberWithInt:_videoSize.width], kCVPixelBufferWidthKey,
                                                           [NSNumber numberWithInt:_videoSize.height], kCVPixelBufferHeightKey,
                                                           nil];
    //    NSDictionary *sourcePixelBufferAttributesDictionary = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt:kCVPixelFormatType_32ARGB], kCVPixelBufferPixelFormatTypeKey,
    //                                                           nil];
    
    _assetWriterPixelBufferInput = [AVAssetWriterInputPixelBufferAdaptor assetWriterInputPixelBufferAdaptorWithAssetWriterInput:_assetWriterVideoInput sourcePixelBufferAttributes:sourcePixelBufferAttributesDictionary];
    
    [_assetWriter addInput:_assetWriterVideoInput];
}

- (void)teardownVideoWriter {
    
}

- (void)setupAudioWriter {
    AVAudioSession *sharedAudioSession = [AVAudioSession sharedInstance];
    double preferredHardwareSampleRate;
    
    if ([sharedAudioSession respondsToSelector:@selector(sampleRate)]) {
        preferredHardwareSampleRate = [sharedAudioSession sampleRate];
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        preferredHardwareSampleRate = [[AVAudioSession sharedInstance] currentHardwareSampleRate];
#pragma clang diagnostic pop
    }
    
    AudioChannelLayout acl;
    bzero( &acl, sizeof(acl));
    acl.mChannelLayoutTag = kAudioChannelLayoutTag_Mono;
    
    if (_audioOutputSettings == nil) {
        _audioOutputSettings = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                [ NSNumber numberWithInt: kAudioFormatMPEG4AAC], AVFormatIDKey,
                                [ NSNumber numberWithInt: 1 ], AVNumberOfChannelsKey,
                                [ NSNumber numberWithFloat: preferredHardwareSampleRate ], AVSampleRateKey,
                                [ NSData dataWithBytes: &acl length: sizeof( acl ) ], AVChannelLayoutKey,
                                //[ NSNumber numberWithInt:AVAudioQualityLow], AVEncoderAudioQualityKey,
                                [ NSNumber numberWithInt: 64000 ], AVEncoderBitRateKey,
                                nil];
    }
    
    
    /*
     AudioChannelLayout acl;
     bzero( &acl, sizeof(acl));
     acl.mChannelLayoutTag = kAudioChannelLayoutTag_Mono;
     
     audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:
     [ NSNumber numberWithInt: kAudioFormatMPEG4AAC ], AVFormatIDKey,
     [ NSNumber numberWithInt: 1 ], AVNumberOfChannelsKey,
     [ NSNumber numberWithFloat: 44100.0 ], AVSampleRateKey,
     [ NSNumber numberWithInt: 64000 ], AVEncoderBitRateKey,
     [ NSData dataWithBytes: &acl length: sizeof( acl ) ], AVChannelLayoutKey,
     nil];*/
    
    _assetWriterAudioInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio
                                                                outputSettings:_audioOutputSettings];
    _assetWriterAudioInput.expectsMediaDataInRealTime = YES;
    [_assetWriter addInput:_assetWriterAudioInput];
}

- (void)teardownAudioWriter {
    
}

- (void)setup {
    [self finish];
    if ([self filter]->srcFramebuffer()) {
        LOG(DEBUG) << "framebuffer size: " << [self filter]->srcFramebuffer()->width() << " " << [self filter]->srcFramebuffer()->height();
    }
    LOG(DEBUG) << "video size: " << _videoSize.width << " " << _videoSize.height;
    
    NSError *error = nil;
    _assetWriter = [[AVAssetWriter alloc] initWithURL:_movieURL fileType:_fileType error:&error];
    if (error != nil)
    {
        const char *err = [[error description] cStringUsingEncoding:NSUTF8StringEncoding];
        LOG(ERROR) << err;
        return ;
    }
    
    // Set this to make sure that a functional movie is produced, even if the recording is cut off mid-stream. Only the last second should be lost in that case.
    _assetWriter.movieFragmentInterval = CMTimeMakeWithSeconds(1.0, 1000);
    
    [self setupVideoWriter];
    [self setupAudioWriter];
}

- (void)teardown {
    [self teardownVideoWriter];
    [self teardownAudioWriter];
}

- (void)start {
    [self setup];
    
    [_assetWriter startWriting];
}

- (void)finish {
    if (_assetWriter.status == AVAssetWriterStatusWriting) {
        [_assetWriterVideoInput markAsFinished];
    }

    [_assetWriter finishWritingWithCompletionHandler:^{
        
    }];
    
    [self teardown];
}

- (void)cancel {
    if (_assetWriter.status == AVAssetWriterStatusCompleted) {
        return;
    }
    
    if (_assetWriter.status == AVAssetWriterStatusWriting) {
        [_assetWriterVideoInput markAsFinished];
    }

    [_assetWriter cancelWriting];
    
    [self teardown];
}

- (const GLchar *)fragmentShader {
    return kColorSwizzlingFragmentShader;
}

- (void)notifyFramebufferToFilters:(int64_t)timestamp {
    dispatch_sync(_movieWriterQueue, ^{
        CMTime frameTime = CMTimeMake(timestamp, 1000);
        if (CMTIME_IS_INVALID(_startTime)) {
            if (_assetWriter.status != AVAssetWriterStatusWriting) {
                [self start];
            }

            _startTime = frameTime;
            [_assetWriter startSessionAtSourceTime:_startTime];
        }
        
        CVPixelBufferRef pixelBuffer = NULL;
        CVReturn status = CVPixelBufferPoolCreatePixelBuffer(NULL, [_assetWriterPixelBufferInput pixelBufferPool], &pixelBuffer);
        if ((pixelBuffer == NULL) || (status != kCVReturnSuccess)) {
            LOG(WARNING) << "Create Pixel buffer failed " << status;
            CVPixelBufferRelease(pixelBuffer);
            return;
        } else {
            CVPixelBufferLockBaseAddress(pixelBuffer, 0);
            
            GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(pixelBuffer);
            glReadPixels(0, 0, _videoSize.width, _videoSize.height,
                         GL_RGBA, GL_UNSIGNED_BYTE, pixelBufferData);
        }
        
        if (!_assetWriterVideoInput.readyForMoreMediaData) {
            LOG(DEBUG) << "Assert writer did not ready for more media data: " << timestamp;
        } else if(_assetWriter.status == AVAssetWriterStatusWriting) {
            if ([_assetWriterPixelBufferInput appendPixelBuffer:pixelBuffer withPresentationTime:frameTime]) {
                LOG(TRACE) << "Appending pixel buffer at time: " << timestamp;
            } else {
                LOG(WARNING) << "Problem appending pixel buffer at time: " << timestamp;
            }
        } else {
            LOG(WARNING) << "Couldn't write a frame at time: " << timestamp;
        }
        
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
        CVPixelBufferRelease(pixelBuffer);
        
        _previousFrameTime = frameTime;
    });
}

- (void)processAudioBuffer:(CMSampleBufferRef)audioBuffer {
    dispatch_sync(_movieWriterQueue, ^{
        CMTime currentSampleTime = CMSampleBufferGetOutputPresentationTimeStamp(audioBuffer);
        if (CMTIME_IS_INVALID(_startTime)) {
            if (_assetWriter.status != AVAssetWriterStatusWriting) {
                [self start];
            }
            _startTime = currentSampleTime;
            [_assetWriter startSessionAtSourceTime:currentSampleTime];
        }
        _previousAudioTime = currentSampleTime;
        
        if (!_assetWriterAudioInput.readyForMoreMediaData) {
            LOG(DEBUG) << "2: Had to drop an audio frame " << CMTimeGetSeconds(currentSampleTime);
        } else if(_assetWriter.status == AVAssetWriterStatusWriting) {
            if (![_assetWriterAudioInput appendSampleBuffer:audioBuffer]) {
                LOG(TRACE) << "Appending audio buffer at time: " << CMTimeGetSeconds(currentSampleTime);
            } else {
                LOG(WARNING) << "Problem appending audio buffer at time: " << CMTimeGetSeconds(currentSampleTime);
            }
        }else {
            LOG(WARNING) << "Couldn't write audio sample at time: " << CMTimeGetSeconds(currentSampleTime);
        }
    });
}

@end
