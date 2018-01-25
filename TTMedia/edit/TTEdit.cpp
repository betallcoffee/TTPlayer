//
//  TTEdit.cpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTEdit.hpp"
#include "TTMutex.hpp"

#include "TTHTTPStream.hpp"
#include "TTFFDemuxer.hpp"
#include "TTAudioCodec.hpp"
#include "TTVideoCodec.hpp"

#include "TTFilter.hpp"

using namespace TT;

const static int kMaxPacketCount = 300;
const static int kMaxFrameCount = 30;

Edit::Edit() : _url(nullptr),
_previews("video_preview_frame", kMaxFrameCount),
_vPacketQueue("video_packet_queue", kMaxPacketCount), _aPacketQueue("audio_packet_queue", kMaxPacketCount),
_vFrameQueue("video_frame_queue", kMaxFrameCount), _aFrameQueue("audio_frame_queue", kMaxFrameCount),
_inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER),
_demuxCond(PTHREAD_COND_INITIALIZER), _demuxMutex(PTHREAD_MUTEX_INITIALIZER),
_audioCond(PTHREAD_COND_INITIALIZER), _audioMutex(PTHREAD_MUTEX_INITIALIZER), _audioDecoding(false),
_videoCond(PTHREAD_COND_INITIALIZER), _videoMutex(PTHREAD_MUTEX_INITIALIZER), _videoDecoding(false) {
     av_register_all();
     avformat_network_init();
     el::Loggers::setLoggingLevel(el::Level::Info);
     LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
     pthread_create(&_inputThread, nullptr, Edit::inputThreadEntry, this);
     pthread_create(&_demuxThread, nullptr, Edit::demuxThreadEntry, this);
     pthread_create(&_audioThread, nullptr, Edit::audioThreadEntry, this);
     pthread_create(&_videoThread, nullptr, Edit::videoThreadEntry, this);
}

Edit::~Edit() {
}

void Edit::start(std::shared_ptr<URL> url) {
    if (_status == kNone ||
        _status == kStoped) {
        _url = url;
        setStatus(kOpen);
    }
}

void Edit::stop() {
    setStatus(kClose);
    _aPacketQueue.clear();
    _vPacketQueue.clear();
}

int Edit::previewCount() {
    return (int)_previews.size();
}

std::shared_ptr<Frame> Edit::preview(int index) {
    if (0 <= index && index < previewCount()) {
        return _previews[index];
    } else {
        return nullptr;
    }
}

void Edit::setStatus(eEditStatus status) {
    Mutex m(&_statusMutex);
    LOG(DEBUG) << "Change status " << _status << " to " << status;
    switch (status) {
        case kOpen:
        {
            if (_status == kNone ||
                _status == kError ||
                _status == kStoped) {
                _status = kOpen;
            }
            break;
        }
        case kClose:
        {
            if (_status == kError ||
                _status == kOpen ||
                _status == kPlaying ||
                _status == kPaused) {
                _status = kClose;
            }
            break;
        }
        case kPlaying:
        {
            if (_status == kOpen ||
                _status == kPaused) {
                _status = kPlaying;
            }
            break;
        }
        case kPaused:
        {
            if (_status == kPlaying) {
                _status = kPaused;
            }
            break;
        }
        case kStoped:
        {
            if (_status == kClose) {
                _status = kStoped;
            }
            break;
        }
        case kQuit:
        {
            _status = kQuit;
            break;
        }
            
        default:
            break;
    }
    pthread_cond_broadcast(&_statusCond);
}

void Edit::waitStatusChange() {
    Mutex m(&_statusMutex);
    pthread_cond_wait(&_statusCond, &_statusMutex);
}

bool Edit::open() {
    if (_status == kOpen) {
        _demuxer = std::make_shared<FFDemuxer>();
        _demuxer->open(_url);
        
        if (_demuxer->hasAudio()) {
            _audioCodec = std::make_shared<AudioCodec>(_demuxer->audioStream());
//            _audioCodec->setCodecCallback(std::bind(&Player::audioCodecCB, this, std::placeholders::_1));
            _audioCodec->open();
        }
        
        if (_demuxer->hasVideo()) {
            _videoCodec = std::make_shared<VideoCodec>(_demuxer->videoStream());
            _videoCodec->open();
        }
        
        setStatus(kPlaying);
    }
    
    return true;;
}

bool Edit::close() {
    if (_status == kClose) {
        LOG(DEBUG) << "Waiting audio/video decode stop.";
        
        _aPacketQueue.clear();
        _vPacketQueue.clear();
        _aFrameQueue.clear();
        _vFrameQueue.clear();
        
        Mutex d(&_demuxMutex);
        while (_demuxing) {
            pthread_cond_wait(&_demuxCond, &_demuxMutex);
        }
        
        Mutex a(&_audioMutex);
        while (_audioDecoding) {
            pthread_cond_wait(&_audioCond, &_audioMutex);
        }
        
        Mutex v(&_videoMutex);
        while (_videoDecoding) {
            pthread_cond_wait(&_videoCond, &_videoMutex);
        }
        
        _aPacketQueue.clear();
        _vPacketQueue.clear();
        _aFrameQueue.clear();
        _vFrameQueue.clear();
        
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
        
        setStatus(kStoped);
    }
    
    return true;
}

void Edit::quit() {
    setStatus(kQuit);
    pthread_join(_inputThread, nullptr);
}

bool Edit::isQuit() {
    Mutex m(&_statusMutex);
    while (_status != kOpen &&
           _status != kPlaying &&
           _status != kClose &&
           _status != kQuit) {
        pthread_cond_wait(&_statusCond, &_statusMutex);
        LOG(DEBUG) << "thread loop wakeup: status:" << _status;
    }
    
    if (_status == kQuit) {
        return true;
    } else {
        return false;
    }
}

void *Edit::inputThreadEntry(void *arg) {
    Edit *self = (Edit *)arg;
    self->inputLoop();
    return nullptr;
}

void Edit::inputLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kOpen:
                open();
                break;
            case kClose:
                close();
                break;
            default:
            {
                usleep(1000);
                break;
            }
        }
    }
}

void *Edit::demuxThreadEntry(void *arg) {
    Edit *self = (Edit *)arg;
    self->demuxLoop();
    return nullptr;
}

void Edit::demuxLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlaying:
            {
                std::shared_ptr<Packet> packet = _demuxer->read();
                if (packet) {
                    switch (packet->type) {
                        case kPacketTypeAudio:
                            _aPacketQueue.push(packet);
                            break;
                        case kPacketTypeVideo:
                            _vPacketQueue.push(packet);
                            break;
                        default:
                            break;
                    }
                } else {
                    usleep(1000);
                }
                
                _demuxing = false;
                pthread_cond_broadcast(&_demuxCond);
                
                break;
            }
            default:
            {
                waitStatusChange();
                break;
            }
        }
    }
}

void *Edit::audioThreadEntry(void *arg) {
    Edit *self = (Edit *)arg;
    self->audioLoop();
    return nullptr;
}

void Edit::audioLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlaying:
            {
                Mutex m(&_audioMutex);
                _audioDecoding = true;
                std::shared_ptr<Packet> packet = _aPacketQueue.pop();
                if (packet && _audioCodec) {
                    std::shared_ptr<Frame> frame;
                    frame = _audioCodec->decode(packet);
                    if (frame) {
                        _aFrameQueue.push(frame);
                    }
                }
                _audioDecoding = false;
                pthread_cond_broadcast(&_audioCond);
                break;
            }
            default:
            {
                waitStatusChange();
                break;
            }
        }
    }
}

void *Edit::videoThreadEntry(void *arg) {
    Edit *self = (Edit *)arg;
    self->videoLoop();
    return nullptr;
}

void Edit::videoLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlaying:
            {
                Mutex m(&_videoMutex);
                _videoDecoding = true;
                std::shared_ptr<Packet> packet = _vPacketQueue.pop();
                if (packet && _videoCodec) {
                    std::shared_ptr<Frame> frame;
                    frame = _videoCodec->decode(packet);
                    if (frame) {
                        // Reorder with pts for B frame.
                        LOG(TRACE) << "Reorder begin " << frame->pts;
                        _vFrameQueue.insert(frame, [&](std::shared_ptr<Frame> l, std::shared_ptr<Frame> r) -> bool {
                            LOG(TRACE) << "Reorder " << l->pts << " " << r->pts;
                            return l->pts <= r->pts;
                        });
                        LOG(TRACE) << "Reorder end " << frame->pts;
                    }
                }
                _videoDecoding = false;
                pthread_cond_broadcast(&_videoCond);
                break;
            }
            default:
            {
                waitStatusChange();
                break;
            }
        }
    }
}
