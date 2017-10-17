//
//  TTAudioQueue_ios.hpp
//  TTPlayerExample
//
//  Created by liang on 10/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTAudioQueue_ios_hpp
#define TTAudioQueue_ios_hpp

#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudioTypes.h>
#import <AudioToolbox/AudioToolbox.h>

#include "TTAudioQueue.hpp"

namespace TT {
    class AudioQueue_ios : public AudioQueue {
    public:
        AudioQueue_ios() {}
        ~AudioQueue_ios() {}
        
        bool setup(AudioDesc &desc);
        void teardown();
        
        void start();
        void stop();
        
        void pause();
        void resume();
        
    private:
        static void queueCallback(void *inUserData, AudioQueueRef inAudioQueue, AudioQueueBufferRef inBuffer);
        void _queueCallback(void *inUserData, AudioQueueRef inAudioQueue, AudioQueueBufferRef inBuffer);
        
    private:
        const static int kNumberOfAudioDataBuffers = 5;
        
        AudioStreamBasicDescription _audioFormat;
        AudioQueueRef _playQueue;
        AudioQueueBufferRef _playQueueBuffers[kNumberOfAudioDataBuffers];
        
    };
}

#endif /* TTAudioQueue_ios_hpp */
