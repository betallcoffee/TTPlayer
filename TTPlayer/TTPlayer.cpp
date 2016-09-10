//
//  TTPlayer.cpp
//  TTPlayerExample
//
//  Created by liang on 8/21/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTPlayer.hpp"
#include "TTURL.hpp"
#include "TTFFStream.hpp"
#include "TTVideoCodec.hpp"

using namespace TT;

Player::Player() : _vPacketQueue("video_packet_queue"), _aPacketQueue("audio_packet_queue"),
_vFrameQueue("video_frame_queue"),
_inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER),
_videoCond(PTHREAD_COND_INITIALIZER), _videoMutex(PTHREAD_MUTEX_INITIALIZER),
_status(kPlayerNone), _videoPause(false) {
    av_register_all();
    avformat_network_init();
    printf("ffmpeg build configure: %s\n", avcodec_configuration());
    pthread_create(&_inputThread, nullptr, Player::inputThreadEntry, this);
    pthread_create(&_videoThread, nullptr, Player::videoThreadEntry, this);
}

Player::~Player() {
    stop();
}

void Player::play(shared_ptr<URL> url) {
    pthread_mutex_lock(&_inputMutex);
    
    _stream = std::make_shared<FFStream>();
    _stream->open(url);
    
    _videoCodec = std::make_shared<VideoCodec>(_stream->videoStream());
    _videoCodec->open();
    
    _status = kPlayerPlaying;
    
    pthread_cond_broadcast(&_inputCond);
    pthread_mutex_unlock(&_inputMutex);
}

void Player::stop() {
    pthread_mutex_lock(&_inputMutex);
    _status = kPlayerQuit;
    pthread_cond_broadcast(&_inputCond);
    pthread_mutex_unlock(&_inputMutex);
    pthread_join(_inputThread, nullptr);
    _stream->close();
    
    _vPacketQueue.close();
    pthread_join(_videoThread, nullptr);
    _videoCodec->close();
}

void *Player::inputThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->inputLoop();
    return nullptr;
}

void Player::inputLoop() {
    while (true) {
        pthread_mutex_lock(&_inputMutex);
        while (_status != kPlayerPlaying && _status != kPlayerQuit) {
            pthread_cond_wait(&_inputCond, &_inputMutex);
        }
        if (_status == kPlayerQuit) {
            pthread_mutex_unlock(&_inputMutex);
            break;
        }
        
        std::shared_ptr<Packet> packet = _stream->read();
        if (packet) {
            switch (packet->type) {
                case kPacketTypeAudio:
                    _aPacketQueue.push(packet);
                    break;
                case kPacketTypeVideo:
                    printf("video packet : %lld\n", packet->pts);
                    _vPacketQueue.push(packet);
                    break;
                default:
                    break;
            }
        }
        
        pthread_mutex_unlock(&_inputMutex);
    }
}

void Player::inputQuit() {
    
}

void *Player::videoThreadEntry(void *arg) {
    Player *self = (Player *)arg;
    self->videoLoop();
    return nullptr;
}

void Player::videoLoop() {
    while (true) {
        pthread_mutex_lock(&_videoMutex);
        while (_videoPause) {
            pthread_cond_wait(&_videoCond, &_videoMutex);
        }
        
        std::shared_ptr<Packet> packet = _vPacketQueue.pop();
        if (packet) {
            std::shared_ptr<Frame> frame = _videoCodec->decode(packet);
            if (frame) {
                printf("video frame: %lld\n", frame->pts);
                _vFrameQueue.push(frame);
            }
        } else {
            pthread_mutex_unlock(&_videoMutex);
            break;
        }
        
        pthread_mutex_unlock(&_videoMutex);
    }
}

void Player::videoQuit() {
    
}




