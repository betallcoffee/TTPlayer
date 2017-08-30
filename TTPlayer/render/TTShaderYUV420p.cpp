//
//  TTRenderYUV420p.cpp
//  TTPlayerExample
//
//  Created by liang on 16/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTShaderYUV420p.hpp"

const static GLint kInvalid = 0;

static const GLfloat kColorConversionMatrix709[] = {
    1.164,  1.164,  1.164,
    0.0,   -0.213,  2.112,
    1.793, -0.533,  0.0,
};

static const GLfloat kColorConversionMatrix601[] = {
    1.164,  1.164, 1.164,
    0.0,   -0.392, 2.017,
    1.596, -0.813, 0.0,
};

static const GLchar *const yuvFragmentShaderString = STRINGIZE
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

using namespace TT;

ShaderYUV420p::ShaderYUV420p() {
    _fragmentShader = yuvFragmentShaderString;
    _type = kTextureTypeY420p;
    memset(_textures, 0, sizeof(_textures));
}

bool ShaderYUV420p::resolveUniforms(GLuint program) {
    if (!Shader::resolveUniforms(program)) {
        return false;
    }
    
    if (program != kInvalid) {
        _uniformSamplers[0] = glGetUniformLocation(program, "s_texture_y");
        _uniformSamplers[1] = glGetUniformLocation(program, "s_texture_u");
        _uniformSamplers[2] = glGetUniformLocation(program, "s_texture_v");
        
        _uniformColorConvertionMatrix = glGetUniformLocation(program, "colorConversionMatrix");
        return true;
    }
    
    return false;
}

bool ShaderYUV420p::uploadTexture(std::shared_ptr<Frame> frame) {
    if (frame == nullptr) {
        return false;
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    if (0 == _textures[0])
        glGenTextures(3, _textures);
    
    const size_t frameHeight = frame->height;
    const size_t heights[3] = { frameHeight, frameHeight / 2, frameHeight / 2 };
    
    for (int i = 0; i < frame->numOfPlanars; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
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
        glUniform1i(_uniformSamplers[i], i);
    }
    glUniformMatrix3fv(_uniformColorConvertionMatrix, 1, GL_FALSE, kColorConversionMatrix601);

    return true;
}

