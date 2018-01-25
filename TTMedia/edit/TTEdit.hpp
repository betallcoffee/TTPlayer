//
//  TTEdit.hpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTEdit_hpp
#define TTEdit_hpp

#include <pthread.h>
#include <vector>

#include "TTArray.hpp"
#include "TTQueue.hpp"
#include "TTURL.hpp"

#include "TTAudioQueue.hpp"
#include "TTRender.hpp"
#include "TTY420ToRGBFilter.hpp"

namespace TT {
    
    class Packet;
    class Frame;
    class Stream;
    class FFDemuxer;
    class AudioCodec;
    class VideoCodec;
    
    typedef enum {
        kNone,
        kError,
        kOpen,
        kClose,
        kPlaying,
        kPaused,
        kStoped,
        kQuit,
    }eEditStatus;
    
    class Edit {
    public:
        Edit();
        ~Edit();
        
        void start(std::shared_ptr<URL> url);
        void stop();
        
        int previewCount();
        std::shared_ptr<Frame> preview(int index);
        
    private:
        void setStatus(eEditStatus status);
        void waitStatusChange();
        
        bool open();
        bool close();
        
        void quit();
        bool isQuit();
        
        static void *inputThreadEntry(void *arg);
        void inputLoop();
        
        static void *demuxThreadEntry(void *arg);
        void demuxLoop();
        
        static void *videoThreadEntry(void *arg);
        void videoLoop();
        
        static void *audioThreadEntry(void *arg);
        void audioLoop();
        
    private:
        std::shared_ptr<URL> _url;
        Array<std::shared_ptr<Frame>> _previews;
        
        eEditStatus _status;
        pthread_cond_t _statusCond;
        pthread_mutex_t _statusMutex;
        
        std::shared_ptr<Stream> _stream;
        
        std::shared_ptr<FFDemuxer> _demuxer;
        Queue<std::shared_ptr<Packet>> _vPacketQueue;
        Queue<std::shared_ptr<Packet>> _aPacketQueue;
        
        std::shared_ptr<AudioCodec> _audioCodec;
        Queue<std::shared_ptr<Frame>> _aFrameQueue;
        
        std::shared_ptr<VideoCodec> _videoCodec;
        Queue<std::shared_ptr<Frame>> _vFrameQueue;
        
        pthread_t _inputThread;
        pthread_cond_t _inputCond;
        pthread_mutex_t _inputMutex;
        
        pthread_t _demuxThread;
        pthread_cond_t _demuxCond;
        pthread_mutex_t _demuxMutex;
        bool _demuxing;
        
        pthread_t _audioThread;
        pthread_cond_t _audioCond;
        pthread_mutex_t _audioMutex;
        bool _audioDecoding;
        
        pthread_t _videoThread;
        pthread_cond_t _videoCond;
        pthread_mutex_t _videoMutex;
        bool _videoDecoding;
    };
}

#endif /* TTEdit_hpp */
