//
//  TTPlayer.hpp
//  TTPlayerExample
//
//  Created by liang on 8/21/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTPlayer_hpp
#define TTPlayer_hpp

#include <pthread.h>

#include "TTQueue.hpp"
#include "TTURL.hpp"
#include "TTCond.hpp"
#include "TTClock.hpp"

#include "TTAudioQueue.hpp"
#include "TTRender.hpp"

namespace TT {
    class FFStream;
    class Packet;
    class AudioCodec;
    class VideoCodec;
    class Frame;
    
    typedef enum {
        kPlayerNone,
        kPlayerPlaying,
        kPlayerPaused,
        kPlayerStoped,
        kPlayerQuit,
    }ePlayerStatus;
    
    class Player {
    public:
        Player();
        ~Player();
        
        void play(std::shared_ptr<URL> url);
        void stop();
        
        void pause();
        void resume();
        
        void seek();
        
        void bindRenderContext(const RenderContext &context);
        void bindAudioQueue(std::shared_ptr<AudioQueue> audioQueue);
        
    private:
        void quit();
        bool isQuit();
        
        static void *inputThreadEntry(void *arg);
        void inputLoop();
        
        static void *videoThreadEntry(void *arg);
        void videoLoop();
        
        static void *audioThreadEntry(void *arg);
        void audioLoop();
        
        static void *renderThreadEntry(void *arg);
        void renderLoop();
        
        void audioCodecCB(AudioDesc &desc);
        std::shared_ptr<Frame> audioQueueCB();
        
        int getMasterSyncType();
        double getMasterClock();
        
    private:
        ePlayerStatus _status;
        pthread_cond_t _statusCond;
        pthread_mutex_t _statusMutex;
        
        std::shared_ptr<FFStream> _stream;
        Queue<std::shared_ptr<Packet>> _vPacketQueue;
        Queue<std::shared_ptr<Packet>> _aPacketQueue;
        
        std::shared_ptr<AudioCodec> _audioCodec;
        Queue<std::shared_ptr<Frame>> _aFrameQueue;
        
        std::shared_ptr<VideoCodec> _videoCodec;
        Queue<std::shared_ptr<Frame>> _vFrameQueue;
        
        pthread_t _inputThread;
        pthread_cond_t _inputCond;
        pthread_mutex_t _inputMutex;
        
        pthread_t _audioThread;
        pthread_cond_t _audioCond;
        pthread_mutex_t _audioMutex;
        
        pthread_t _videoThread;
        pthread_cond_t _videoCond;
        pthread_mutex_t _videoMutex;
        
        pthread_t _renderThread;
        
        std::shared_ptr<AudioQueue> _audioQueue;
        Render _render;
        
        AVSyncClock _clock;
        Clock _aClock;
        Clock _vClock;
        Clock _eClock;
        int64_t _vPTS;
    };
}

#endif /* TTPlayer_hpp */