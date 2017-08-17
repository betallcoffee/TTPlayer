//
//  TTFramebuffer.cpp
//  TTPlayerExample
//
//  Created by liang on 14/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTFramebuffer.hpp"

using namespace TT;

Framebuffer::Framebuffer() : _width(0), _height(0), _framebuffer(0), _texture(0) {
    
}

Framebuffer::~Framebuffer() {
    tearDown();
}

bool Framebuffer::setUp(size_t width, size_t height) {
    if (_framebuffer) {
        if (width == _width && height == _height) {
            return true;
        } else {
            tearDown();
        }
    }
    
    // Set up framebuffer
    if (_framebuffer == 0)
        glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    
    glActiveTexture(GL_TEXTURE1);
    if (_texture == 0)
        glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }
    
    _width = width;
    _height = height;

    return true;
}

void Framebuffer::tearDown() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if (_framebuffer) {
        glDeleteFramebuffers(1, &_framebuffer);
        _framebuffer = 0;
    }
    if (_texture) {
        glDeleteTextures(1, &_texture);
        _texture = 0;
    }
}

void Framebuffer::active() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glViewport(0, 0, _width, _height);
}


