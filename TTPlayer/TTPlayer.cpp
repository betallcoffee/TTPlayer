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

#include "TTFFStream.hpp"
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
#define AV_NOSYNC_THRESHOLD 100.0

Player::Player() : _status(kPlayerNone),
 _statusCond(PTHREAD_COND_INITIALIZER), _statusMutex(PTHREAD_MUTEX_INITIALIZER),
 _vPacketQueue("video_packet_queue", kMaxPacketCount), _aPacketQueue("audio_packet_queue", kMaxPacketCount),
 _vFrameQueue("video_frame_queue", kMaxFrameCount), _aFrameQueue("audio_frame_queue"),
 _inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER),
 _audioCond(PTHREAD_COND_INITIALIZER), _audioMutex(PTHREAD_MUTEX_INITIALIZER),
 _videoCond(PTHREAD_COND_INITIALIZER), _videoMutex(PTHREAD_MUTEX_INITIALIZER),
 _clock(AV_SYNC_AUDIO_MASTER) {
     av_register_all();
     avformat_network_init();
     el::Loggers::setLoggingLevel(el::Level::Info);
     LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
     pthread_create(&_inputThread, nullptr, Player::inputThreadEntry, this);
     pthread_create(&_audioThread, nullptr, Player::audioThreadEntry, this);
     pthread_create(&_videoThread, nullptr, Player::videoThreadEntry, this);
     pthread_create(&_renderThread, nullptr, Player::renderThreadEntry, this);
}

Player::~Player() {
    stop();
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
    Mutex m(&_statusMutex);
    
    if (_status == kPlayerPlaying ||
        _status == kPlayerPaused ||
        _status == kPlayerQuit) {
        return;
    }
    
    _stream = std::make_shared<FFStream>();
    _stream->open(url);
    
    if (_stream->hasAudio()) {
        _audioCodec = std::make_shared<AudioCodec>(_stream->audioStream());
        _audioCodec->setCodecCallback(std::bind(&Player::audioCodecCB, this, std::placeholders::_1));
        _audioCodec->open();
    }
    
    if (_stream->hasVideo()) {
        _videoCodec = std::make_shared<VideoCodec>(_stream->videoStream());
        _videoCodec->open();
    }
    
    _status = kPlayerPlaying;
    
    pthread_cond_broadcast(&_statusCond);
}

void Player::stop() {
    Mutex m(&_statusMutex);
    if (_status != kPlayerPlaying && _status != kPlayerPaused) {
        return;
    }
    _status = kPlayerStoped;

    _stream->close();
    
    _aPacketQueue.close();
    _audioCodec->close();
    
    _vPacketQueue.close();
    _videoCodec->close();
}

void Player::pause() {
    Mutex m(&_statusMutex);
    if (_status != kPlayerPlaying) {
        return;
    }
    _status = kPlayerPaused;
}

void Player::resume() {
    Mutex m(&_statusMutex);
    if (_status != kPlayerPaused) {
        return;
    }
    _status = kPlayerPlaying;
}

void Player::quit() {
    do {
        Mutex m(&_statusMutex);
        _status = kPlayerQuit;
        pthread_cond_broadcast(&_inputCond);
    } while (0);
    pthread_join(_inputThread, nullptr);
}

bool Player::isQuit() {
    Mutex m(&_statusMutex);
    while (_status != kPlayerPlaying && _status != kPlayerQuit) {
        pthread_cond_wait(&_statusCond, &_statusMutex);
        LOG(INFO) << "thread loop wakeup: status:" << _status;
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
    
        std::shared_ptr<Packet> packet = _stream->read();
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
    }
}

void *Player::videoThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->videoLoop();
    return nullptr;
}

void Player::videoLoop() {
    while (!isQuit()) {
        std::shared_ptr<Packet> packet = _vPacketQueue.pop();
        if (packet && _videoCodec) {
            std::shared_ptr<Frame> frame = _videoCodec->decode(packet);
            if (frame) {
                _vFrameQueue.push(frame);
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
        std::shared_ptr<Packet> packet = _aPacketQueue.pop();
        if (packet && _audioCodec) {
            std::shared_ptr<Frame> frame = _audioCodec->decode(packet);
            if (frame) {
                _aFrameQueue.push(frame);
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
        std::shared_ptr<Frame> frame = _vFrameQueue.pop();
        if (frame) {
            int64_t delay = frame->pts - _vPTS;
            LOG(TRACE) << "render delay1 " << delay;
            /* update delay to follow master synchronisation source */
            if (getMasterSyncType() != AV_SYNC_VIDEO_MASTER) {
                /* if video is slave, we try to correct big delays by
                 duplicating or deleting a frame */
                int64_t diff = _vClock.getClock() - getMasterClock();
                LOG(TRACE) << "render diff " << diff;
                /* skip or repeat frame. We take into account the
                 delay to compute the threshold. I still don't know
                 if it is the best guess */
                int64_t sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
                if (!isnan(diff) && abs(diff) < AV_NOSYNC_THRESHOLD) {
                    if (diff <= -sync_threshold)
                        delay = FFMAX(0, delay + diff);
                    else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
                        delay = delay + diff;
                    else if (diff >= sync_threshold)
                        delay = 2 * delay;
                }
            }
            
            LOG(TRACE) << "render video frame " << frame->pts;
//            _render.displayFrame(frame);
            _filter.updateFrame(frame);
            _vPTS = frame->pts;
            _vClock.setClock(frame->pts);
            LOG(TRACE) << "render delay2 " << delay;
            usleep(delay * 1000);
        }
    }
}

void Player::audioCodecCB(TT::AudioDesc &desc) {
    if (_audioQueue) {
        _audioQueue->setUp(desc);
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

int Player::getMasterSyncType() {
    if (_clock == AV_SYNC_VIDEO_MASTER) {
        if (_stream->hasVideo())
            return AV_SYNC_VIDEO_MASTER;
        else
            return AV_SYNC_AUDIO_MASTER;
    } else if (_clock == AV_SYNC_AUDIO_MASTER) {
        if (_stream->hasAudio())
            return AV_SYNC_AUDIO_MASTER;
        else
            return AV_SYNC_EXTERNAL_CLOCK;
    } else {
        return AV_SYNC_EXTERNAL_CLOCK;
    }
}

double Player::getMasterClock() {
    double val;
    switch (getMasterSyncType()) {
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

