//
//  TTSource.cpp
//  TTPlayerExample
//
//  Created by liang on 13/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTSource.hpp"
#include "TTFramebuffer.hpp"

using namespace TT;
/*
Source::Source() : _frameBuffer(nullptr) {
    _frameBuffer = std::make_shared<FrameBuffer>();
}

Source::~Source() {
    _frameBuffer.reset();
    removeAllTargets();
}

void Source::addTarget(std::shared_ptr<Target> target, int index) {
    if (target && index >= 0) {
        _targets[index] = target;
    }
}

void Source::removeTarget(std::shared_ptr<Target> target, int index) {
    if (target && index >= 0) {
        _targets.erase(index);
    }
}

void Source::removeAllTargets() {
    std::map<int, std::shared_ptr<Target>>::iterator it;
    for (it = _targets.begin(); it != _targets.end(); it++) {
        it->second.reset();
    }
    _targets.clear();
}

void Source::sendDataToTargets() {
    std::map<int, std::shared_ptr<Target>>::iterator it;
    for (it = _targets.begin(); it != _targets.end(); it++) {
        it->second->setSrcFrameBuffer(_frameBuffer);
    }
}

void Source::notifyFrameBufferToTargets() {
    std::map<int, std::shared_ptr<Target>>::iterator it;
    for (it = _targets.begin(); it != _targets.end(); it++) {
        it->second->render();
    }
}*/
