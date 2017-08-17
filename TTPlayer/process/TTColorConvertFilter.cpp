//
//  TTColorConvertFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 17/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTColorConvertFilter.hpp"

#include "TTGLContext_ios.h"
#include "TTFramebuffer.hpp"

using namespace TT;

static const GLfloat kColorConversionMatrix601[] = {
    1.164,  1.164, 1.164,
    0.0,   -0.392, 2.017,
    1.596, -0.813, 0.0,
};

static const GLchar *const kVertexShader = STRINGIZE
(
 attribute vec4 position;
 attribute vec2 texcoord;
 varying vec2 v_texcoord;
 
 void main()
 {
     gl_Position = position;
     v_texcoord = texcoord.xy;
 }
 );

static const GLchar *const kYUVFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 uniform sampler2D s_texture_y;
 uniform sampler2D s_texture_u;
 uniform sampler2D s_texture_v;
 uniform mat3 colorConversionMatrix;
 
 void main() {
     mediump vec3 yuv;
     lowp vec3 rgb;
     
     yuv.x = texture2D(s_texture_y, v_texcoord).r - (16.0/255.0);
     yuv.y = texture2D(s_texture_u, v_texcoord).r - 0.5;
     yuv.z = texture2D(s_texture_v, v_texcoord).r - 0.5;
     
     rgb = colorConversionMatrix * yuv;
     gl_FragColor = vec4(rgb, 1.0);
 }
 );

ColorConvertFilter::ColorConvertFilter() {
}

ColorConvertFilter::~ColorConvertFilter() {
    
}

void ColorConvertFilter::compileShader() {
    GLContext::sharedProcessContext().use();
    _program.compile(kVertexShader, kYUVFragmentShader);
    
    _positionLocation = _program.getAttribLocation("position");
    _texCoordLocation = _program.getAttribLocation("texcoord");
    _uniformSamplers[0] = _program.getUniformLocation("s_texture_y");
    _uniformSamplers[1] = _program.getUniformLocation("s_texture_u");
    _uniformSamplers[2] = _program.getUniformLocation("s_texture_v");
    
    _uniformColorConvertionMatrix = _program.getUniformLocation("colorConversionMatrix");
}

void ColorConvertFilter::updateFrame(std::shared_ptr<Frame> frame) {
    if (frame) {
        GLContext::sharedProcessContext().use();
        
        if (_framebuffer->setUp(frame->width, frame->height)){
            _framebuffer->active();
            if (!_program.isCompiled())
                compileShader();
            _program.use();
            
            updateTexture(frame);
            
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            const GLfloat *vertices = Program::positionVertices();
            glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, 0, 0, vertices);
            glEnableVertexAttribArray(_positionLocation);
            
            const GLfloat *texCoord = Program::texCoordForRotation(kTexNoRotation);
            glVertexAttribPointer(_texCoordLocation, 2, GL_FLOAT, 0, 0, texCoord);
            glEnableVertexAttribArray(_texCoordLocation);
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
            notifyFramebufferToFilters(frame->pts);
        }
    }
}

void ColorConvertFilter::updateTexture(std::shared_ptr<Frame> frame) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    if (0 == _textures[0])
        glGenTextures(3, _textures);
    
    const size_t frameHeight = frame->height;
    const size_t heights[3] = { frameHeight, frameHeight / 2, frameHeight / 2 };
    
    for (int i = 0; i < frame->numOfPlanars; ++i) {
        glActiveTexture(GL_TEXTURE2 + i);
        glBindTexture(GL_TEXTURE_2D, _textures[i]);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_LUMINANCE,
                     frame->lineSize[i],
                     heights[i],
                     0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE,
                     frame->data[i]);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(_uniformSamplers[i], i + 2);
    }
    glUniformMatrix3fv(_uniformColorConvertionMatrix, 1, GL_FALSE, kColorConversionMatrix601);
}
