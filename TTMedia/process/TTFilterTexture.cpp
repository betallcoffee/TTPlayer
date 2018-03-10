//
//  TTFilterTexture.cpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTFilterTexture.hpp"
#include "TTGLContext_ios.h"

using namespace TT;

FilterTexture::FilterTexture() {
    
}

FilterTexture::~FilterTexture() {
    
}

void FilterTexture::process(int64_t timestamp) {
    TIMED_FUNC(timer);
    GLContext::sharedProcessContext().use();
    
    PERFORMANCE_CHECKPOINT(timer);
    if (bindFramebuffer()) {
        updateTexture();
        PERFORMANCE_CHECKPOINT(timer);
        notifyFramebufferToFilters(timestamp);
        PERFORMANCE_CHECKPOINT(timer);
    }
}
