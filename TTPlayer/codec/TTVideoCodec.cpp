//
//  TTVideoCodec.cpp
//  TTPlayerExample
//
//  Created by liang on 9/9/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTVideoCodec.hpp"

using namespace TT;

VideoCodec::VideoCodec(AVStream *avStream) : _avStream(avStream), _avCodecContext(avStream->codec) {
}

VideoCodec::~VideoCodec() {
    if (_avCodecContext) {
        avcodec_close(_avCodecContext);
        _avCodecContext = nullptr;
        _avCodec = nullptr;
    }
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
        _avCodec = nullptr;
    }
}

std::shared_ptr<Frame> VideoCodec::decode(std::shared_ptr<Packet> packet) {
    AVFrame *avframe = av_frame_alloc();
    int len, got_frame;
    len = avcodec_decode_video2(_avCodecContext, avframe, &got_frame, packet->avpacket());
    
    if (len < 0) {
        printf("soft decoder fail (%d)\n", len);
        av_frame_free(&avframe);
    } else if (got_frame) {
        std::shared_ptr<Frame> frame = std::make_shared<Frame>(avframe);
        frame->pts = avframe->pts == AV_NOPTS_VALUE ? packet->dts  : avframe->pts;
        frame->type = kTextureTypeY420p;
        AVRational tb = _avStream->time_base;
        frame->pts = frame->pts * av_q2d(tb) * 1000;
        return frame;
    }
    
    return nullptr;
}


