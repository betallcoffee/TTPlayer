//
//  TTContrastFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 18/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTContrastFilter.hpp"

using namespace TT;

static const GLchar *const kContrastFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 uniform float contrast;
 
 void main()
 {
     vec4 textureColor = texture2D(texture, v_texcoord);
     gl_FragColor = vec4(((textureColor.rgb - vec3(0.5)) * contrast + vec3(0.5)), textureColor.w);
 }
 );

ContrastFilter::ContrastFilter() : _contrast(1.0), _contrastUniform(kInvalid) {
    
}

ContrastFilter::~ContrastFilter() {
    
}

void ContrastFilter::setContrast(float contrast) {
    _contrast = contrast;
}

const GLchar *ContrastFilter::fragmentShader() {
    return kContrastFragmentShader;
}

void ContrastFilter::getUniformLocations() {
    Filter::getUniformLocations();
    _contrastUniform = _program.getUniformLocation("contrast");
}

void ContrastFilter::resolveUniformLocations() {
    Filter::resolveUniformLocations();
    glUniform1f(_contrastUniform, _contrast);
}



