//
//  TTFilterFrame.cpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTFilterFrame.hpp"

using namespace TT;

FilterFrame::FilterFrame() : _frame(nullptr) {
    
}

FilterFrame::~FilterFrame() {
}

void FilterFrame::processFrame(std::shared_ptr<Frame> frame) {
    if (frame) {
        _frame = frame;
        _width = _frame->width;
        _height = _frame->height;
        process(frame->pts);
        _frame = nullptr;
    }
}
