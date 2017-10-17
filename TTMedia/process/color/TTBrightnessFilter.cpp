//
//  TTBrightnessFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 29/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTBrightnessFilter.hpp"

using namespace TT;

static const GLchar *const kBrightnessFragmentShader = STRINGIZE
( 
 precision highp float;
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 uniform float brightness;
 
 void main()
 {
     vec4 textureColor = texture2D(texture, v_texcoord);
     gl_FragColor = vec4((textureColor.rgb + vec3(brightness)), textureColor.w);
 }
 );


BrightnessFilter::BrightnessFilter() : _brightness(0.0), _brightnessUniform(kInvalid) {
    
}

BrightnessFilter::~BrightnessFilter() {
    
}

void BrightnessFilter::setBrightness(float brightness) {
    _brightness = brightness;
}

const GLchar *BrightnessFilter::fragmentShader() {
    return kBrightnessFragmentShader;
}

void BrightnessFilter::getUniformLocations() {
    Filter::getUniformLocations();
    _brightnessUniform = _program.getUniformLocation("brightness");
}

void BrightnessFilter::resolveUniformLocations() {
    Filter::resolveUniformLocations();
    glUniform1f(_brightnessUniform, _brightness);
}


