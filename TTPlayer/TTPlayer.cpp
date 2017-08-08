//
//  TTPlayer.cpp
//  TTPlayerExample
//
//  Created by liang on 8/21/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTPlayer.hpp"

#include "TTMutex.hpp"
#include "TTURL.hpp"

#include "TTFFStream.hpp"
#include "TTVideoCodec.hpp"

using namespace TT;

Player::Player() : _status(kPlayerNone),
_statusCond(PTHREAD_COND_INITIALIZER), _statusMutex(PTHREAD_MUTEX_INITIALIZER),
_vPacketQueue("video_packet_queue"), _aPacketQueue("audio_packet_queue"),
_vFrameQueue("video_frame_queue"),
_inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER),
_videoCond(PTHREAD_COND_INITIALIZER), _videoMutex(PTHREAD_MUTEX_INITIALIZER) {
    av_register_all();
    avformat_network_init();
    printf("ffmpeg build configure: %s\n", avcodec_configuration());
    pthread_create(&_inputThread, nullptr, Player::inputThreadEntry, this);
    pthread_create(&_videoThread, nullptr, Player::videoThreadEntry, this);
    pthread_create(&_renderThread, nullptr, Player::renderThreadEntry, this);
}

Player::~Player() {
    stop();
}

void Player::bindRenderContext(const RenderContext *context) {
    _render.bindContext(context);
}

void Player::play(shared_ptr<URL> url) {
    Mutex m(&_statusMutex);
    
    if (_status == kPlayerPlaying ||
        _status == kPlayerPaused ||
        _status == kPlayerQuit) {
        return;
    }
    
    _stream = std::make_shared<FFStream>();
    _stream->open(url);
    
    _videoCodec = std::make_shared<VideoCodec>(_stream->videoStream());
    _videoCodec->open();
    
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
        printf("thread loop wakeup: status(%d)\n", _status);
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
//                    printf("video packet : %lld\n", packet->pts);
                    _vPacketQueue.push(packet);
                    break;
                default:
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
        
        std::shared_ptr<Packet> packet = _vPacketQueue.pop();
        if (packet) {
            std::shared_ptr<Frame> frame = _videoCodec->decode(packet);
            if (frame) {
//                printf("video frame: %lld\n", frame->pts);
                _vFrameQueue.push(frame);
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
            printf("render frame: %lld\n", frame->pts);
            _render.displayFrame(frame);
            usleep(33 * 1000);
        }
    }
}




