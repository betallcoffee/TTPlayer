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

Frame::Frame() : _avFrame(nullptr),
 type(kAudioPCM),
 numOfPlanars(0),
 width(0), height(0),
 pts(AV_NOPTS_VALUE), pkt_pts(AV_NOPTS_VALUE), pkt_dts(AV_NOPTS_VALUE),
 sampleFormat(AV_SAMPLE_FMT_NONE) {
    memset(data, 0, sizeof(data));
    memset(lineSize, 0, sizeof(lineSize));
}

Frame::Frame(AVFrame *avFrame) : _avFrame(avFrame),
 type(kTextureTypeY420p),
 numOfPlanars(kNumOfPlanars),
 width(avFrame->width), height(avFrame->height),
 pts(avFrame->pts), pkt_pts(avFrame->pkt_pts), pkt_dts(avFrame->pkt_dts),
 sampleFormat(AV_SAMPLE_FMT_NONE) {
    memcpy(data, _avFrame->data, numOfPlanars * sizeof(data[0]));
    memcpy(lineSize, _avFrame->linesize, numOfPlanars * sizeof(lineSize[0]));
}

Frame::~Frame() {
    for (int i = 0; i < kNumOfPlanars; i++) {
        if (data[i]) {
            if (_avFrame == nullptr) {
                free(data[i]);
            }
            data[i] = nullptr;
        }
    }
    if (_avFrame) {
        av_frame_unref(_avFrame);
        av_frame_free(&_avFrame);
        _avFrame = nullptr;
    }
}

bool Frame::reallocData(size_t dataSize, int index) {
    if (dataSize <= 0 || index < 0 || index >= kNumOfPlanars) {
        return false;
    }
    
    if (data[index]) {
        free(data[index]);
        data[index] = nullptr;
    }
    
    data[index] = (uint8_t *)malloc(dataSize);
    if (data[index] == nullptr) {
        return false;
    }
    lineSize[index] = dataSize;
    
    return true;
}
