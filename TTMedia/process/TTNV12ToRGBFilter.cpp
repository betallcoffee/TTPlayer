//
//  TTNV12ToRGBFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTNV12ToRGBFilter.hpp"
#include "TTProcessPriv.h"

using namespace TT;

static const GLchar *const kNV12FragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 uniform sampler2D s_texture_y;
 uniform sampler2D s_texture_uv;
 uniform mat3 colorConversionMatrix;
 
 void main() {
     mediump vec3 yuv;
     lowp vec3 rgb;
     
     yuv.x = texture2D(s_texture_y, v_texcoord).r - (16.0/255.0);
     yuv.y = texture2D(s_texture_uv, v_texcoord).r - 0.5;
     yuv.z = texture2D(s_texture_uv, v_texcoord).g - 0.5;
     
     rgb = colorConversionMatrix * yuv;
     gl_FragColor = vec4(rgb, 1.0);
 }
 );

NV12ToRGBFilter::NV12ToRGBFilter() : _uniformColorConvertionMatrix(0) {
    memset(_uniformSamplers, 0, sizeof(_uniformSamplers));
    memset(_textures, 0, sizeof(_textures));
}

NV12ToRGBFilter::~NV12ToRGBFilter() {
    if (_textures[0]) {
        glDeleteTextures(3, _textures);
    }
}

const GLchar *NV12ToRGBFilter::fragmentShader() {
    return kNV12FragmentShader;
}

void NV12ToRGBFilter::getUniformLocations() {
    _uniformSamplers[0] = _program.getUniformLocation("s_texture_y");
    _uniformSamplers[1] = _program.getUniformLocation("s_texture_uv");
    _uniformColorConvertionMatrix = _program.getUniformLocation("colorConversionMatrix");
}

void NV12ToRGBFilter::updateTexture() {
    if (_frame == nullptr) {
        return;
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    if (0 == _textures[0])
        glGenTextures(2, _textures);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textures[0]);
    const size_t lumaStride = _frame->lineSize[0];
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED_EXT,
                 lumaStride,
                 _height,
                 0,
                 GL_RED_EXT,
                 GL_UNSIGNED_BYTE,
                 _frame->data[0]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textures[0]);
    glUniform1i(_uniformSamplers[0], 0);
    
    if(_frame->numOfPlanars == 2) {
        // UV-plane.
        glActiveTexture(GL_TEXTURE1);
        
        glBindTexture(GL_TEXTURE_2D, _textures[1]);
        const size_t chromaStride = _frame->lineSize[1];
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RG_EXT,
                     chromaStride / 2,
                     _height / 2,
                     0,
                     GL_RG_EXT,
                     GL_UNSIGNED_BYTE,
                     _frame->data[1]);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(_uniformSamplers[1], 1);
    }
    
    glUniformMatrix3fv(_uniformColorConvertionMatrix, 1, GL_FALSE, kColorConversionMatrix601);
}
