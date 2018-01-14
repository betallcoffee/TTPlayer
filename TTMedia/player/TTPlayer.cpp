//
//  TTPlayer.cpp
//  TTPlayerExample
//
//  Created by liang on 8/21/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <stdlib.h>

#include "easylogging++.h"

#include "TTPlayer.hpp"

#include "TTMutex.hpp"
#include "TTURL.hpp"

#include "TTHTTPStream.hpp"
#include "TTFFDemuxer.hpp"
#include "TTAudioCodec.hpp"
#include "TTVideoCodec.hpp"

#include "TTFilter.hpp"

using namespace TT;

INITIALIZE_EASYLOGGINGPP

const static int kMaxPacketCount = 300;
const static int kMaxFrameCount = 3;

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 40
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 100
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 150
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10000.0

Player::Player() : _status(kPlayerNone),
 _statusCond(PTHREAD_COND_INITIALIZER), _statusMutex(PTHREAD_MUTEX_INITIALIZER),
 _vPacketQueue("video_packet_queue", kMaxPacketCount), _aPacketQueue("audio_packet_queue", kMaxPacketCount),
 _vFrameQueue("video_frame_queue", kMaxFrameCount), _aFrameQueue("audio_frame_queue", kMaxFrameCount),
 _inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER),
 _demuxCond(PTHREAD_COND_INITIALIZER), _demuxMutex(PTHREAD_MUTEX_INITIALIZER),
 _audioCond(PTHREAD_COND_INITIALIZER), _audioMutex(PTHREAD_MUTEX_INITIALIZER), _audioDecoding(false),
 _videoCond(PTHREAD_COND_INITIALIZER), _videoMutex(PTHREAD_MUTEX_INITIALIZER), _videoDecoding(false),
 _renderCond(PTHREAD_COND_INITIALIZER), _renderMutex(PTHREAD_MUTEX_INITIALIZER), _renderring(false),
 _clock(AV_SYNC_AUDIO_MASTER) {
     av_register_all();
     avformat_network_init();
     el::Loggers::setLoggingLevel(el::Level::Info);
     LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
     pthread_create(&_inputThread, nullptr, Player::inputThreadEntry, this);
     pthread_create(&_demuxThread, nullptr, Player::demuxThreadEntry, this);
     pthread_create(&_audioThread, nullptr, Player::audioThreadEntry, this);
     pthread_create(&_videoThread, nullptr, Player::videoThreadEntry, this);
     pthread_create(&_renderThread, nullptr, Player::renderThreadEntry, this);
}

Player::~Player() {
    close();
}

void Player::bindRenderContext(const RenderContext &context) {
    _render.bindContext(context);
}

void Player::bindAudioQueue(std::shared_ptr<AudioQueue> audioQueue) {
    _audioQueue = audioQueue;
    if (_audioQueue) {
        _audioQueue->setQueueCallback(std::bind(&Player::audioQueueCB, this));
    }
}

void Player::bindFilter(std::shared_ptr<Filter> filter) {
    _filter.addFilter(filter);
}

void Player::play(std::shared_ptr<URL> url) {
    if (_status == kPlayerNone ||
        _status == kPlayerStoped) {
        _url = url;
        setStatus(kPlayerOpen);
    }
}

void Player::stop() {
    setStatus(kPlayerClose);
    _aPacketQueue.clear();
    _vPacketQueue.clear();
}

void Player::pause() {
    setStatus(kPlayerPaused);
}

void Player::resume() {
    setStatus(kPlayerPlaying);
}

void Player::setStatus(ePlayerStatus status) {
    Mutex m(&_statusMutex);
    LOG(DEBUG) << "Change status " << _status << " to " << status;
    switch (status) {
        case kPlayerOpen:
        {
            if (_status == kPlayerNone ||
                _status == kPlayerError ||
                _status == kPlayerStoped) {
                _status = kPlayerOpen;
            }
            break;
        }
        case kPlayerClose:
        {
            if (_status == kPlayerError ||
                _status == kPlayerOpen ||
                _status == kPlayerPlaying ||
                _status == kPlayerPaused) {
                _status = kPlayerClose;
            }
            break;
        }
        case kPlayerPlaying:
        {
            if (_status == kPlayerOpen ||
                _status == kPlayerPaused) {
                _status = kPlayerPlaying;
            }
            break;
        }
        case kPlayerPaused:
        {
            if (_status == kPlayerPlaying) {
                _status = kPlayerPaused;
            }
            break;
        }
        case kPlayerStoped:
        {
            if (_status == kPlayerClose) {
                _status = kPlayerStoped;
            }
            break;
        }
        case kPlayerQuit:
        {
            _status = kPlayerQuit;
            break;
        }
            
        default:
            break;
    }
    pthread_cond_broadcast(&_statusCond);
}

void Player::waitStatusChange() {
    Mutex m(&_statusMutex);
    pthread_cond_wait(&_statusCond, &_statusMutex);
}

bool Player::open() {
    if (_status == kPlayerOpen) {
        openStream();
        
        _vPTS = 0;
        _aClock.reset();
        _vClock.reset();
        _eClock.reset();
        
        _demuxer = std::make_shared<FFDemuxer>();
        _demuxer->open(_url);
        
        if (_demuxer->hasAudio()) {
            _audioCodec = std::make_shared<AudioCodec>(_demuxer->audioStream());
            _audioCodec->setCodecCallback(std::bind(&Player::audioCodecCB, this, std::placeholders::_1));
            _audioCodec->open();
        }
        
        if (_demuxer->hasVideo()) {
            _videoCodec = std::make_shared<VideoCodec>(_demuxer->videoStream());
            _videoCodec->open();
        }
        
        setMasterSyncType(AV_SYNC_AUDIO_MASTER);
        setStatus(kPlayerPlaying);
    }
    
    return true;;
}

bool Player::openStream() {
    _stream = std::make_shared<HTTPStream>();
    return _stream->open(_url, 0, 0);
}

bool Player::close() {
    if (_status == kPlayerClose) {
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
        
        Mutex r(&_renderMutex);
        while (_renderring) {
            pthread_cond_wait(&_renderCond, &_renderMutex);
        }
        
        _aPacketQueue.clear();
        _vPacketQueue.clear();
        _aFrameQueue.clear();
        _vFrameQueue.clear();
        
        _vPTS = 0;
        _aClock.reset();
        _vClock.reset();
        _eClock.reset();
        
        LOG(DEBUG) << "Audio/Video decode stopped.";
        
        if (_audioQueue) {
            _audioQueue->teardown();
        }
        
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
        
        setStatus(kPlayerStoped);
    }
    
    return true;
}

void Player::quit() {
    setStatus(kPlayerQuit);
    pthread_join(_inputThread, nullptr);
}

bool Player::isQuit() {
    Mutex m(&_statusMutex);
    while (_status != kPlayerOpen &&
           _status != kPlayerPlaying &&
           _status != kPlayerClose &&
           _status != kPlayerQuit) {
        pthread_cond_wait(&_statusCond, &_statusMutex);
        LOG(DEBUG) << "thread loop wakeup: status:" << _status;
    }
    
    if (_status == kPlayerQuit) {
        return true;
    } else {
        return false;
    }
}

void *Player::inputThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->inputLoop();
    return nullptr;
}

void Player::inputLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlayerOpen:
                open();
                break;
            case kPlayerClose:
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

void *Player::demuxThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->demuxLoop();
    return nullptr;
}

void Player::demuxLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlayerPlaying:
            {
                Mutex m(&_demuxMutex);
                _demuxing = true;
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

void *Player::audioThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->audioLoop();
    return nullptr;
}

void Player::audioLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlayerPlaying:
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

void *Player::videoThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->videoLoop();
    return nullptr;
}

void Player::videoLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlayerPlaying:
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

void *Player::renderThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->renderLoop();
    return nullptr;
}

void Player::renderLoop() {
    while (!isQuit()) {
        switch (_status) {
            case kPlayerPlaying:
            {
                Mutex m(&_renderMutex);
                _renderring = true;
                std::shared_ptr<Frame> frame = _vFrameQueue.pop();
                if (frame) {
                    int64_t delay = frame->pts - _vPTS;
                    LOG(TRACE) << "render delay1 " << delay;
                    /* update delay to follow master synchronisation source */
                    if (_clock != AV_SYNC_VIDEO_MASTER) {
                        /* if video is slave, we try to correct big delays by
                         duplicating or deleting a frame */
                        int64_t diff = _vClock.getClock() - getMasterClock();
                        LOG(TRACE) << "render diff " << diff;
                        /* skip or repeat frame. We take into account the
                         delay to compute the threshold. I still don't know
                         if it is the best guess */
                        int64_t syncThreshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
                        LOG(TRACE) << "sync threshold " << syncThreshold;
                        if (!isnan(diff) && abs(diff) < AV_NOSYNC_THRESHOLD) {
                            if (diff <= -syncThreshold)
                                delay = FFMAX(0, delay + diff);
                            else if (diff >= syncThreshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
                                delay = delay + diff;
                            else if (diff >= syncThreshold)
                                delay = 2 * delay;
                        } else {
                            LOG(WARNING) << "diff is lager " << diff;
                        }
                    }
                    
                    LOG(TRACE) << "render video frame " << frame->pts;
//                    _render.displayFrame(frame);
                    _filter.processFrame(frame);
                    _vPTS = frame->pts;
                    _vClock.setClock(frame->pts);
                    LOG(TRACE) << "render delay2 " << delay;
                    if (delay < 0) {
                        LOG(WARNING) << "delay is negative " << delay;
                        delay = 0;
                    }
                    usleep(delay * 1000);
                }
                _renderring = false;
                pthread_cond_broadcast(&_renderCond);
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

void Player::audioCodecCB(TT::AudioDesc &desc) {
    if (_audioQueue) {
        _audioQueue->setup(desc);
    }
}

std::shared_ptr<Frame> Player::audioQueueCB() {
    if (_aFrameQueue.empty()) {
        return nullptr;
    } else {
        std::shared_ptr<Frame> frame = _aFrameQueue.pop();
        if (frame) {
            _aClock.setClock(frame->pts);
            LOG(TRACE) << "render audio frame " << frame->pts;
        }
        
        return frame;
    }
}

void Player::setMasterSyncType(AVSyncClock clock) {
    if (clock == AV_SYNC_VIDEO_MASTER) {
        if (_demuxer->hasVideo())
            _clock = AV_SYNC_VIDEO_MASTER;
        else
            _clock = AV_SYNC_AUDIO_MASTER;
    } else if (clock == AV_SYNC_AUDIO_MASTER) {
        if (_demuxer->hasAudio())
            _clock = AV_SYNC_AUDIO_MASTER;
        else
            _clock = AV_SYNC_EXTERNAL_CLOCK;
    } else {
        _clock = AV_SYNC_EXTERNAL_CLOCK;
    }
}

double Player::getMasterClock() {
    double val;
    switch (_clock) {
        case AV_SYNC_VIDEO_MASTER:
            val = _vClock.getClock();
            break;
        case AV_SYNC_AUDIO_MASTER:
            val = _aClock.getClock();
            break;
        default:
            val = _eClock.getClock();
            break;
    }
    return val;
}

