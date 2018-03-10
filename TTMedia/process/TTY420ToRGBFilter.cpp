//
//  TTY420ToRGBFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 17/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTY420ToRGBFilter.hpp"
#include "TTProcessPriv.h"

using namespace TT;

static const GLchar *const kY420FragmentShader = STRINGIZE
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

Y420ToRGBFilter::Y420ToRGBFilter() : _uniformColorConvertionMatrix(0) {
    memset(_uniformSamplers, 0, sizeof(_uniformSamplers));
    memset(_textures, 0, sizeof(_textures));
}

Y420ToRGBFilter::~Y420ToRGBFilter() {
    if (_textures[0]) {
        glDeleteTextures(3, _textures);
    }
}

const GLchar *Y420ToRGBFilter::fragmentShader() {
    return kY420FragmentShader;
}

void Y420ToRGBFilter::getUniformLocations() {
    _uniformSamplers[0] = _program.getUniformLocation("s_texture_y");
    _uniformSamplers[1] = _program.getUniformLocation("s_texture_u");
    _uniformSamplers[2] = _program.getUniformLocation("s_texture_v");
    _uniformColorConvertionMatrix = _program.getUniformLocation("colorConversionMatrix");
}

void Y420ToRGBFilter::updateTexture() {
    if (_frame == nullptr) {
        return;
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    if (0 == _textures[0])
        glGenTextures(3, _textures);
    
    const size_t frameHeight = _frame->height;
    const size_t heights[3] = { frameHeight, frameHeight / 2, frameHeight / 2 };
    
    for (int i = 0; i < _frame->numOfPlanars; ++i) {
        glActiveTexture(GL_TEXTURE2 + i);
        glBindTexture(GL_TEXTURE_2D, _textures[i]);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_LUMINANCE,
                     _frame->lineSize[i],
                     heights[i],
                     0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE,
                     _frame->data[i]);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(_uniformSamplers[i], i + 2);
    }
    glUniformMatrix3fv(_uniformColorConvertionMatrix, 1, GL_FALSE, kColorConversionMatrix601);
}
