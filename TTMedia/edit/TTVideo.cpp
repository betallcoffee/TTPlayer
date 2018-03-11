//
//  TTVideoEdit.cpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTVideo.hpp"
#include "TTMutex.hpp"

#include "TTHTTPStream.hpp"
#include "TTFFDemuxer.hpp"
#include "TTAudioCodec.hpp"
#include "TTVideoCodec.hpp"

#include "TTFFWriter.hpp"

using namespace TT;

const static int kMaxPacketCount = 300;
const static int kMaxFrameCount = 0;

Video::Video() :
_previews("video_preview_frame", kMaxFrameCount),
_status(VideoStatus::kNone), _statusCond(PTHREAD_COND_INITIALIZER), _statusMutex(PTHREAD_MUTEX_INITIALIZER),
_statusCallback(nullptr), _eventCallback(nullptr), _readFrameCallback(nullptr),
_stream(nullptr), _demuxer(nullptr), _writer(nullptr),
_vPacketQueue("video_packet_queue", kMaxPacketCount), _aPacketQueue("audio_packet_queue", kMaxPacketCount),
_vFrameArray("video_frame_array", kMaxFrameCount), _aFrameQueue("audio_frame_queue", kMaxFrameCount),
_vPacketArray("video_packet_array", kMaxFrameCount),
_inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER),
_demuxMutex(PTHREAD_MUTEX_INITIALIZER),
_audioMutex(PTHREAD_MUTEX_INITIALIZER),
_videoMutex(PTHREAD_MUTEX_INITIALIZER) {
    av_register_all();
    avformat_network_init();
    el::Loggers::setLoggingLevel(el::Level::Debug);
    LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
    pthread_create(&_inputThread, nullptr, Video::inputThreadEntry, this);
}

Video::~Video() {
}

void Video::setStatusCallback(StatusCallback cb) {
    Mutex m(&_statusMutex);
    _statusCallback = cb;
}

void Video::setEventCallback(EventCallback cb) {
    Mutex m(&_statusMutex);
    _eventCallback = cb;
}

void Video::setReadFrameCallback(ReadFrameCallback cb) {
    Mutex m(&_videoMutex);
    _readFrameCallback = cb;
}

void Video::start(std::shared_ptr<URL> url) {
    if (_status == VideoStatus::kNone ||
        _status == VideoStatus::kStoped) {
        _url = url;
        setStatus(VideoStatus::kOpen);
    }
}

void Video::stop() {
    setStatus(VideoStatus::kClose);
    _aPacketQueue.clear();
    _vPacketQueue.clear();
}

void Video::save(std::shared_ptr<URL> url) {
    _saveUrl = url;
    setStatus(VideoStatus::kWrite);
}

int Video::previewCount() {
    return (int)_previews.size();
}

std::shared_ptr<Frame> Video::preview(int index) {
    if (0 <= index && index < previewCount()) {
        return _previews[index];
    }
    return nullptr;
}

int Video::frameCount() {
    return (int)_vFrameArray.size();
}

std::shared_ptr<Frame> Video::frame(int index) {
    if (0 <= index && index < frameCount()) {
        return _vFrameArray[index];
    }
    return nullptr;
}

void Video::setStatus(VideoStatus status) {
    Mutex m(&_statusMutex);
    LOG(DEBUG) << "Will change status " << (int)_status << " to " << (int)status;
    VideoStatus oldStatus = _status;
    switch (status) {
        case VideoStatus::kOpen:
        {
            if (_status == VideoStatus::kNone ||
                _status == VideoStatus::kError ||
                _status == VideoStatus::kStoped) {
                _status = VideoStatus::kOpen;
            }
            break;
        }
        case VideoStatus::kClose:
        {
            if (_status == VideoStatus::kError ||
                _status == VideoStatus::kOpen ||
                _status == VideoStatus::kRead ||
                _status == VideoStatus::kEdit ||
                _status == VideoStatus::kWrite ||
                _status == VideoStatus::kPaused) {
                _status = VideoStatus::kClose;
            }
            break;
        }
        case VideoStatus::kRead:
        {
            if (_status == VideoStatus::kOpen) {
                _status = VideoStatus::kRead;
            }
            break;
        }
        case VideoStatus::kEdit:
        {
            if (_status == VideoStatus::kRead ||
                _status == VideoStatus::kWrite) {
                _status = VideoStatus::kEdit;
            }
            break;
        }
        case VideoStatus::kWrite:
        {
            if (_status == VideoStatus::kEdit) {
                _status = VideoStatus::kWrite;
            }
        }
        case VideoStatus::kPaused:
        {
            if (_status == VideoStatus::kEdit) {
                _status = VideoStatus::kPaused;
            }
            break;
        }
        case VideoStatus::kStoped:
        {
            if (_status == VideoStatus::kClose) {
                _status = VideoStatus::kStoped;
            }
            break;
        }
        case VideoStatus::kQuit:
        {
            _status = VideoStatus::kQuit;
            break;
        }
            
        default:
            break;
    }
    if (oldStatus != _status && _statusCallback) {
        LOG(DEBUG) << "Did change status " << (int)oldStatus << " to " << (int)_status;
        _statusCallback(this, _status);
    }
    pthread_cond_broadcast(&_statusCond);
}

void Video::waitStatusChange() {
    Mutex m(&_statusMutex);
    pthread_cond_wait(&_statusCond, &_statusMutex);
}

void *Video::inputThreadEntry(void *arg) {
    pthread_setname_np("edit input thread");
    Video *self = (Video *)arg;
    self->inputLoop();
    return nullptr;
}

void Video::inputLoop() {
    while (!isQuit()) {
        switch (_status) {
            case VideoStatus::kOpen:
            {
                open();
                break;
            }
            case VideoStatus::kClose:
            {
                close();
                break;
            }
            case VideoStatus::kRead:
            {
                read();
                break;
            }
            case VideoStatus::kWrite:
            {
                write();
                break;
            }
            default:
            {
                usleep(1);
                break;
            }
        }
    }
}

bool Video::open() {
    if (_status == VideoStatus::kOpen) {
        _demuxer = std::make_shared<FFDemuxer>();
        _demuxer->open(_url);
        
        if (_demuxer->hasAudio()) {
            _audioCodec = std::make_shared<AudioCodec>(_demuxer->audioStream());
            _audioCodec->open();
        }
        
        if (_demuxer->hasVideo()) {
            _videoCodec = std::make_shared<VideoCodec>(_demuxer->videoStream());
            _videoCodec->open();
        }
        
        setStatus(VideoStatus::kRead);
    }
    
    return true;;
}

bool Video::close() {
    if (_status == VideoStatus::kClose) {
        LOG(DEBUG) << "Waiting audio/video decode stop.";
        
        _aPacketQueue.clear();
        _vPacketQueue.clear();
        _aFrameQueue.clear();
        _vFrameArray.clear();
        
        _aPacketQueue.clear();
        _vPacketQueue.clear();
        _aFrameQueue.clear();
        _vFrameArray.clear();
        
        LOG(DEBUG) << "Audio/Video decode stopped.";
        
        _aPacketQueue.close();
        if (_audioCodec) {
            _audioCodec->close();
            _audioCodec.reset();
        }
        
        _vPacketQueue.close();
        if (_videoCodec) {
            _videoCodec->close();
            _videoCodec.reset();
        }
        
        if (_demuxer) {
            _demuxer->close();
            _demuxer.reset();
        }
        
        if (_writer) {
            _writer->finish();
            _writer.reset();
        }
        
        setStatus(VideoStatus::kStoped);
    }
    
    return true;
}

void Video::quit() {
    setStatus(VideoStatus::kQuit);
    pthread_join(_inputThread, nullptr);
}

bool Video::isQuit() {
    Mutex m(&_statusMutex);
    while (_status != VideoStatus::kOpen &&
           _status != VideoStatus::kRead &&
           _status != VideoStatus::kEdit &&
           _status != VideoStatus::kWrite &&
           _status != VideoStatus::kClose &&
           _status != VideoStatus::kQuit) {
        pthread_cond_wait(&_statusCond, &_statusMutex);
        LOG(DEBUG) << "thread loop wakeup: status:" << (int)_status;
    }
    
    if (_status == VideoStatus::kQuit) {
        return true;
    } else {
        return false;
    }
}

bool Video::read() {
    std::shared_ptr<Packet> packet = _demuxer->read();
    if (packet) {
        switch (packet->type) {
            case kPacketTypeAudio:
//                _aPacketQueue.push(packet);
                break;
            case kPacketTypeVideo:
                videoDecode(packet);
                _vPacketArray.pushBack(packet);
                break;
            default:
                break;
        }
        return true;
    } else if (_demuxer->isEOF()) {
        if (_eventCallback) {
            _eventCallback(this, VideoEvent::kReadEnd);
        }
        setStatus(VideoStatus::kEdit);
        return true;
    }
    
    return false;
}

void Video::videoDecode(std::shared_ptr<Packet> packet) {
    if (packet && _videoCodec) {
        std::shared_ptr<Frame> frame;
        frame = _videoCodec->decode(packet);
        if (frame) {
            // Reorder with pts for B frame.
            LOG(TRACE) << "Reorder begin " << frame->pts << " frame count:" << _vFrameArray.size();
            _vFrameArray.insert(frame, [&](std::shared_ptr<Frame> l, std::shared_ptr<Frame> r) -> bool {
                LOG(TRACE) << "Reorder " << l->pts << " " << r->pts;
                return l->pts <= r->pts;
            });
            
            LOG(TRACE) << "Reorder end " << frame->pts << " frame count:" << _vFrameArray.size();
            
            if (frame->isKeyframe()) {
                LOG(TRACE) << "Decode keyframe pts:" << frame->pts;
                _previews.pushBack(frame);
            }
            
            if (_readFrameCallback) {
                _readFrameCallback(this, _vFrameArray.size());
            }
        }
    }
}

bool Video::write() {
    if (_demuxer && _writer == nullptr) {
        _writer = std::make_shared<FFWriter>();
        AVCodecParameters *audioCodecParam = nullptr;
        AVCodecParameters *videoCodecParam = nullptr;
        if (_demuxer->audioStream()) {
            audioCodecParam = _demuxer->audioStream()->codecpar;
        }
        if (_demuxer->videoStream()) {
            videoCodecParam = _demuxer->videoStream()->codecpar;
        }
        if (!_writer->start(_saveUrl, audioCodecParam, videoCodecParam)) {
            LOG(ERROR) << "Edit muxer open failed:" << _saveUrl;
            _writer->cancel();
            _writer = nullptr;
            setStatus(VideoStatus::kEdit);
            return false;
        }
    }
    
    if (_writer) {
        int count = _vPacketArray.size();
        for (int i = 10; i < count; i++) {
            std::shared_ptr<Packet> packet = _vPacketArray[i];
            _writer->processPacket(packet);
        }
        setStatus(VideoStatus::kClose);
    }
    return true;
}

bool Video::encode() {
    return false;
}
