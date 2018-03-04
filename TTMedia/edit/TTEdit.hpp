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

#include "TTFrame.hpp"
#include "TTAudioQueue.hpp"
#include "TTRender.hpp"
#include "TTY420ToRGBFilter.hpp"

namespace TT {
    
    class Packet;
    class Frame;
    class Stream;
    class FFDemuxer;
    class FFMuxer;
    class AudioCodec;
    class VideoCodec;
    
    typedef enum class EditStatus {
        kNone = 0,
        kError = 1,
        kOpen = 2,
        kClose = 3,
        kDecode = 4,
        kEdit = 5,
        kSave = 6,
        kPaused = 7,
        kStoped = 8,
        kQuit = 9,
    } eEditStatus;
    
    typedef enum class EditEvent {
        kNone,
        kDecodeEnd,
    } eEditEvent;
    
    class Edit {
    public:
        Edit();
        ~Edit();
        
        typedef std::function<void(Edit *, EditStatus)> StatusCallback;
        void setStatusCallback(StatusCallback cb);
        
        typedef std::function<void(Edit *, size_t size)> DecodeFrameCallback;
        void setDecodeFrameCallback(DecodeFrameCallback cb);
        
        typedef std::function<void(Edit *, EditEvent event)> EventCallback;
        void setEventCallback(EventCallback cb);
        
        void start(std::shared_ptr<URL> url);
        void stop();
        
        void done(std::shared_ptr<URL> url);
        
        int previewCount();
        std::shared_ptr<Frame> preview(int index);
        
        int videoFrameCount();
        std::shared_ptr<Frame> videoFrame(int index);
        
    private:
        void setStatus(EditStatus status);
        void waitStatusChange();
        
        bool open();
        bool close();
        bool save();
        
        void quit();
        bool isQuit();
        
        bool decode();
        void videoDecode(std::shared_ptr<Packet> packet);
        
        bool encode();
        
        static void *inputThreadEntry(void *arg);
        void inputLoop();
        
    private:
        std::shared_ptr<URL> _url;
        std::shared_ptr<URL> _saveUrl;
        
        eEditStatus _status;
        pthread_cond_t _statusCond;
        pthread_mutex_t _statusMutex;
        StatusCallback _statusCallback;
        EventCallback _eventCallback;
        
        std::shared_ptr<Stream> _stream;
        pthread_mutex_t _demuxMutex;
        std::shared_ptr<FFDemuxer> _demuxer;
        std::shared_ptr<FFMuxer> _muxer;
        
        Queue<std::shared_ptr<Packet>> _vPacketQueue;
        Queue<std::shared_ptr<Packet>> _aPacketQueue;
        
        pthread_mutex_t _audioMutex;
        std::shared_ptr<AudioCodec> _audioCodec;
        Queue<std::shared_ptr<Frame>> _aFrameQueue;
        
        pthread_mutex_t _videoMutex;
        std::shared_ptr<VideoCodec> _videoCodec;
        Array<std::shared_ptr<Frame>> _vFrameArray;
        Array<std::shared_ptr<Packet>> _vPacketArray;
        Array<std::shared_ptr<Frame>> _previews;
        DecodeFrameCallback _decodeFrameCallback;
        
        pthread_t _inputThread;
        pthread_cond_t _inputCond;
        pthread_mutex_t _inputMutex;
    };
}

#endif /* TTEdit_hpp */
