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
#include "TTFFDemuxer.hpp"

using namespace TT;

Edit::Edit() : _url(nullptr),
 _inputCond(PTHREAD_COND_INITIALIZER), _inputMutex(PTHREAD_MUTEX_INITIALIZER) {
     av_register_all();
     avformat_network_init();
     el::Loggers::setLoggingLevel(el::Level::Info);
     LOG(DEBUG) << "ffmpeg build configure: " << avcodec_configuration();
     pthread_create(&_inputThread, nullptr, Edit::inputThreadEntry, this);
}

Edit::~Edit() {
}

void Edit::open(std::shared_ptr<URL> url) {
    _url = url;
}

void Edit::close() {
    
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

void *Edit::inputThreadEntry(void *arg) {
    Edit *self = (Edit *)arg;
    self->inputLoop();
    return nullptr;
}

void Edit::inputLoop() {
//    while (!isQuit()) {
//        switch (_status) {
//            case kPlayerOpen:
//                open();
//                break;
//            case kPlayerClose:
//                close();
//                break;
//            case kPlayerPlaying:
//            {
//                std::shared_ptr<Packet> packet = _stream->read();
//                if (packet) {
//                    switch (packet->type) {
//                        case kPacketTypeAudio:
//                            _aPacketQueue.push(packet);
//                            break;
//                        case kPacketTypeVideo:
//                            _vPacketQueue.push(packet);
//                            break;
//                        default:
//                            break;
//                    }
//                } else {
//                    usleep(1000);
//                }
//                break;
//            }
//            default:
//            {
//                usleep(1000);
//                break;
//            }
//        }
//    }
}
