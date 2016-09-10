//
//  TTFrame.cpp
//  TTPlayerExample
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <stdlib.h>

#include "TTFrame.hpp"

using namespace TT;

Frame::Frame(AVFrame *avFrame) : _avFrame(avFrame), width(avFrame->width), height(avFrame->height),
pts(avFrame->pts), pkt_pts(avFrame->pkt_pts), pkt_dts(avFrame->pkt_dts),
type(kFrameTypeY420p), numOfPlanars(kNumOfPlanars) {
    memcpy(data, _avFrame->data, numOfPlanars);
    memcpy(lineSize, _avFrame->linesize, numOfPlanars);
}

Frame::~Frame() {
    if (_avFrame) {
        av_frame_free(&_avFrame);
    }
    for (int i = 0; i < kNumOfPlanars; i++) {
        if (data[i]) {
            free(data[i]);
            data[i] = nullptr;
        }
    }
}
