//
//  TTFFWriter.cpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTFFWriter.hpp"

using namespace TT;

FFWriter::FFWriter() {
    
}

FFWriter::~FFWriter() {
    
}

bool FFWriter::start(std::shared_ptr<URL> url, AVCodecParameters *audioCodecParam, AVCodecParameters *videoCodecParam) {
    _url = url;
    _muxer = std::make_shared<FFMuxer>();
    if (!_muxer->open(_url, audioCodecParam, videoCodecParam)) {
        LOG(ERROR) << "Edit muxer open failed:" << _url;
        _muxer->close();
        _muxer = nullptr;
        return false;
    }
    return true;
}

bool FFWriter::finish() {
    if (_muxer) {
        _muxer->close();
    }
    return true;
}

bool FFWriter::cancel() {
    return finish();
}

void FFWriter::processPacket(std::shared_ptr<Packet> packet) {
    if (_muxer) {
        _muxer->write(packet);
    }
}

void FFWriter::processFrame(std::shared_ptr<Frame> frame) {
    
}

void FFWriter::process(int64_t timestamp) {
    
}
