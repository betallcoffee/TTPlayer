//
//  TTGLContext_ios.mm
//  TTPlayerExample
//
//  Created by liang on 16/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include <OpenGLES/EAGL.h>

#include "TTGLContext_ios.h"

using namespace TT;

GLContext::GLContext() {
    _context = (__bridge_retained void *)[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
}

GLContext::~GLContext() {
    if ([EAGLContext currentContext] == _context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    if (_context) {
        CFBridgingRelease(_context);
        _context = nullptr;
    }
}

GLContext &GLContext::sharedProcessContext() {
    // http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
    static GLContext processContext;
    return processContext;
}

void GLContext::use() {
    if ([EAGLContext currentContext] != _context)
    {
        [EAGLContext setCurrentContext:(__bridge EAGLContext *)_context];
    }
    
    // Set up a few global settings for the image processing pipeline
    glDisable(GL_DEPTH_TEST);
}
