//
//  TTVideoEdit.cpp
//  TTPlayerExample
//
//  Created by liang on 17/10/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTVideoEdit.hpp"
#include "TTMutex.hpp"

#include "TTHTTPStream.hpp"
#include "TTFFDemuxer.hpp"
#include "TTAudioCodec.hpp"
#include "TTVideoCodec.hpp"

#include "TTFFWriter.hpp"

using namespace TT;

const static int kMaxPacketCount = 300;
const static int kMaxFrameCount = 0;

VideoEdit::VideoEdit() : _url(nullptr),
_previews("video_preview_frame", kMaxFrameCount),
_status(EditStatus::kNone), _statusCond(PTHREAD_COND_INITIALIZER), _statusMutex(PTHREAD_MUTEX_INITIALIZER),
_statusCallback(nullptr), _eventCallback(nullptr), _decodeFrameCallback(nullptr),
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
    pthread_create(&_inputThread, nullptr, VideoEdit::inputThreadEntry, this);
}

VideoEdit::~VideoEdit() {
}

void VideoEdit::setStatusCallback(StatusCallback cb) {
    Mutex m(&_statusMutex);
    _statusCallback = cb;
}

void VideoEdit::setEventCallback(EventCallback cb) {
    Mutex m(&_statusMutex);
    _eventCallback = cb;
}

void VideoEdit::setDecodeFrameCallback(DecodeFrameCallback cb) {
    Mutex m(&_videoMutex);
    _decodeFrameCallback = cb;
}

void VideoEdit::start(std::shared_ptr<URL> url) {
    if (_status == EditStatus::kNone ||
        _status == EditStatus::kStoped) {
        _url = url;
        setStatus(EditStatus::kOpen);
    }
}

void VideoEdit::stop() {
    setStatus(EditStatus::kClose);
    _aPacketQueue.clear();
    _vPacketQueue.clear();
}

void VideoEdit::done(std::shared_ptr<URL> url) {
    _saveUrl = url;
    setStatus(EditStatus::kSave);
}

int VideoEdit::previewCount() {
    return (int)_previews.size();
}

std::shared_ptr<Frame> VideoEdit::preview(int index) {
    if (0 <= index && index < previewCount()) {
        return _previews[index];
    }
    return nullptr;
}

int VideoEdit::videoFrameCount() {
    return (int)_vFrameArray.size();
}

std::shared_ptr<Frame> VideoEdit::videoFrame(int index) {
    if (0 <= index && index < videoFrameCount()) {
        return _vFrameArray[index];
    }
    return nullptr;
}

void VideoEdit::setStatus(EditStatus status) {
    Mutex m(&_statusMutex);
    LOG(DEBUG) << "Will change status " << (int)_status << " to " << (int)status;
    EditStatus oldStatus = _status;
    switch (status) {
        case EditStatus::kOpen:
        {
            if (_status == EditStatus::kNone ||
                _status == EditStatus::kError ||
                _status == EditStatus::kStoped) {
                _status = EditStatus::kOpen;
            }
            break;
        }
        case EditStatus::kClose:
        {
            if (_status == EditStatus::kError ||
                _status == EditStatus::kOpen ||
                _status == EditStatus::kDecode ||
                _status == EditStatus::kEdit ||
                _status == EditStatus::kSave ||
                _status == EditStatus::kPaused) {
                _status = EditStatus::kClose;
            }
            break;
        }
        case EditStatus::kDecode:
        {
            if (_status == EditStatus::kOpen) {
                _status = EditStatus::kDecode;
            }
            break;
        }
        case EditStatus::kEdit:
        {
            if (_status == EditStatus::kDecode ||
                _status == EditStatus::kSave) {
                _status = EditStatus::kEdit;
            }
            break;
        }
        case EditStatus::kSave:
        {
            if (_status == EditStatus::kEdit) {
                _status = EditStatus::kSave;
            }
        }
        case EditStatus::kPaused:
        {
            if (_status == EditStatus::kEdit) {
                _status = EditStatus::kPaused;
            }
            break;
        }
        case EditStatus::kStoped:
        {
            if (_status == EditStatus::kClose) {
                _status = EditStatus::kStoped;
            }
            break;
        }
        case EditStatus::kQuit:
        {
            _status = EditStatus::kQuit;
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

void VideoEdit::waitStatusChange() {
    Mutex m(&_statusMutex);
    pthread_cond_wait(&_statusCond, &_statusMutex);
}

void *VideoEdit::inputThreadEntry(void *arg) {
    pthread_setname_np("edit input thread");
    VideoEdit *self = (VideoEdit *)arg;
    self->inputLoop();
    return nullptr;
}

void VideoEdit::inputLoop() {
    while (!isQuit()) {
        switch (_status) {
            case EditStatus::kOpen:
            {
                open();
                break;
            }
            case EditStatus::kClose:
            {
                close();
                break;
            }
            case EditStatus::kDecode:
            {
                decode();
                break;
            }
            case EditStatus::kSave:
            {
                save();
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

bool VideoEdit::open() {
    if (_status == EditStatus::kOpen) {
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
        
        setStatus(EditStatus::kDecode);
    }
    
    return true;;
}

bool VideoEdit::close() {
    if (_status == EditStatus::kClose) {
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
        
        setStatus(EditStatus::kStoped);
    }
    
    return true;
}

void VideoEdit::quit() {
    setStatus(EditStatus::kQuit);
    pthread_join(_inputThread, nullptr);
}

bool VideoEdit::isQuit() {
    Mutex m(&_statusMutex);
    while (_status != EditStatus::kOpen &&
           _status != EditStatus::kDecode &&
           _status != EditStatus::kEdit &&
           _status != EditStatus::kSave &&
           _status != EditStatus::kClose &&
           _status != EditStatus::kQuit) {
        pthread_cond_wait(&_statusCond, &_statusMutex);
        LOG(DEBUG) << "thread loop wakeup: status:" << (int)_status;
    }
    
    if (_status == EditStatus::kQuit) {
        return true;
    } else {
        return false;
    }
}

bool VideoEdit::decode() {
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
            _eventCallback(this, EditEvent::kDecodeEnd);
        }
        setStatus(EditStatus::kEdit);
        return true;
    }
    
    return false;
}

void VideoEdit::videoDecode(std::shared_ptr<Packet> packet) {
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
            
            if (_decodeFrameCallback) {
                _decodeFrameCallback(this, _vFrameArray.size());
            }
        }
    }
}

bool VideoEdit::save() {
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
            setStatus(EditStatus::kEdit);
            return false;
        }
    }
    
    if (_writer) {
        int count = _vPacketArray.size();
        for (int i = 10; i < count; i++) {
            std::shared_ptr<Packet> packet = _vPacketArray[i];
            _writer->processPacket(packet);
        }
        setStatus(EditStatus::kClose);
    }
    return true;
}

bool VideoEdit::encode() {
    return false;
}
