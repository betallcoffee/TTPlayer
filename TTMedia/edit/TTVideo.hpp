//
//  TTVideo.hpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTVideoEdit_hpp
#define TTVideoEdit_hpp

#include <pthread.h>
#include <vector>

#include "TTArray.hpp"
#include "TTQueue.hpp"
#include "TTURL.hpp"

#include "TTFilterFrame.hpp"
#include "TTAudioQueue.hpp"
#include "TTRender.hpp"

#include "TTMaterial.hpp"

namespace TT {
    
    class Packet;
    class Frame;
    class Stream;
    class FFDemuxer;
    class FFWriter;
    class AudioCodec;
    class VideoCodec;
    
    typedef enum class VideoStatus {
        kNone = 0,
        kError = 1,
        kOpen = 2,
        kClose = 3,
        kRead = 4,
        kEdit = 5,
        kWrite = 6,
        kPaused = 7,
        kStoped = 8,
        kQuit = 9,
    } eEditStatus;
    
    typedef enum class VideoEvent {
        kNone,
        kReadEnd,
        kWriteEnd,
    } eEditEvent;
    
    class Video : public Material {
    public:
        Video();
        ~Video();
        
        bool process() override;
        
        bool open(std::shared_ptr<URL> url) override;
        bool close() override;
        void save(std::shared_ptr<URL> url) override;
        
        int frameCount() override;
        std::shared_ptr<Frame> frame(int index) override;
        
        typedef std::function<void(Video *, VideoStatus)> StatusCallback;
        void setStatusCallback(StatusCallback cb);
        
        typedef std::function<void(Video *, size_t size)> ReadFrameCallback;
        void setReadFrameCallback(ReadFrameCallback cb);
        
        typedef std::function<void(Video *, VideoEvent event)> EventCallback;
        void setEventCallback(EventCallback cb);
        
        int previewCount();
        std::shared_ptr<Frame> preview(int index);
        
    private:
        void setStatus(VideoStatus status);
        void waitStatusChange();
        
        bool open();
        bool close_();
        bool write();
        
        void quit();
        bool isQuit();
        
        bool read();
        void videoDecode(std::shared_ptr<Packet> packet);
        
        bool encode();
        
        static void *inputThreadEntry(void *arg);
        void inputLoop();
        
    private:
        std::shared_ptr<URL> _saveUrl;
        
        eEditStatus _status;
        pthread_cond_t _statusCond;
        pthread_mutex_t _statusMutex;
        StatusCallback _statusCallback;
        EventCallback _eventCallback;
        
        std::shared_ptr<Stream> _stream;
        pthread_mutex_t _demuxMutex;
        std::shared_ptr<FFDemuxer> _demuxer;
        std::shared_ptr<FFWriter> _writer;
        
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
        ReadFrameCallback _readFrameCallback;
    };
}

#endif /* TTVideoEdit_hpp */
