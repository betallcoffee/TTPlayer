//
//  TTVideoCodec.cpp
//  TTPlayerExample
//
//  Created by liang on 9/9/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTVideoCodec.hpp"

using namespace TT;

VideoCodec::VideoCodec(AVStream *avStream) : _avStream(avStream) {
    if (_avStream) {
        _avCodecContext = _avStream->codec;
    }
}

VideoCodec::~VideoCodec() {
    close();
}

bool VideoCodec::open() {
    if (_avCodecContext) {
        _avCodec = avcodec_find_decoder(_avCodecContext->codec_id);
        if (_avCodec) {
            if (avcodec_open2(_avCodecContext, _avCodec, NULL) == 0) {
                return true;
            }
        }
    }
    return false;
}

void VideoCodec::close() {
    if (_avCodecContext) {
        avcodec_close(_avCodecContext);
        _avCodecContext = nullptr;
        _avCodec = nullptr;
    }
}

std::shared_ptr<Frame> VideoCodec::decode(std::shared_ptr<Packet> packet) {
    AVFrame *avFrame = av_frame_alloc();
    int len, gotFrame;
    len = avcodec_decode_video2(_avCodecContext, avFrame, &gotFrame, packet->avpacket());
    
    if (len < 0) {
        printf("soft decoder fail (%d)\n", len);
        av_frame_free(&avFrame);
    } else if (gotFrame) {
        std::shared_ptr<Frame> frame = std::make_shared<Frame>(avFrame);
        frame->pts = avFrame->pts == AV_NOPTS_VALUE ? packet->dts : avFrame->pts;
        frame->type = kTextureTypeY420p;
        AVRational tb = _avStream->time_base;
        frame->pts = frame->pts * av_q2d(tb) * 1000;
        return frame;
    }
    
    return nullptr;
}


