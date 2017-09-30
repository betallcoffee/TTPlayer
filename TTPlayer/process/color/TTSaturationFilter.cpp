//
//  TTSaturationFilter.cpp
//  TTPlayerExample
//
//  Created by liang on 30/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTSaturationFilter.hpp"

using namespace TT;

static const GLchar *const kSaturationFragmentShader = STRINGIZE
(
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 uniform lowp float saturation;
 
 // Values from "Graphics Shaders: Theory and Practice" by Bailey and Cunningham
 const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);
 
 void main()
 {
     lowp vec4 textureColor = texture2D(texture, v_texcoord);
     lowp float luminance = dot(textureColor.rgb, luminanceWeighting);
     lowp vec3 greyScaleColor = vec3(luminance);
     
     gl_FragColor = vec4(mix(greyScaleColor, textureColor.rgb, saturation), textureColor.w);
 }
 );

SaturationFilter::SaturationFilter() : _saturation(1.0), _saturationUniform(kInvalid) {
    
}

SaturationFilter::~SaturationFilter() {
    
}

void SaturationFilter::setSaturation(float saturation) {
    _saturation = saturation;
}

const GLchar *SaturationFilter::fragmentShader() {
    return kSaturationFragmentShader;
}

void SaturationFilter::getUniformLocations() {
    Filter::getUniformLocations();
    _saturationUniform = _program.getUniformLocation("saturation");
}

void SaturationFilter::resolveUniformLocations() {
    Filter::resolveUniformLocations();
    glUniform1f(_saturationUniform, _saturation);
}

