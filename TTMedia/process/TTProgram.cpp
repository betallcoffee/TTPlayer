//
//  TTProgram.cpp
//  TTPlayerExample
//
//  Created by liang on 16/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTProgram.hpp"

using namespace TT;

const static GLint kInvalid = 0;

Program::Program() : _prog(kInvalid), _vertShader(kInvalid), _fragShader(kInvalid) {
    
}

Program::~Program() {
    reset();
}

void Program::reset() {
    if (_vertShader) {
        glDeleteShader(_vertShader);
        _vertShader = kInvalid;
    }
    if (_fragShader) {
        glDeleteShader(_fragShader);
        _fragShader = kInvalid;
    }
    
    if (_prog) {
        glDeleteProgram(_prog);
        _prog = kInvalid;
    }
}

bool Program::isCompiled() {
    if (_prog != kInvalid) {
        return true;
    }
    return false;
}

bool Program::compile(const GLchar *vertShader, const GLchar *fragShader) {
    reset();
    
    _prog = glCreateProgram();
    if (_prog == kInvalid) {
        reset();
        return false;
    }
    
    _vertShader = compileShader(GL_VERTEX_SHADER, vertShader);
    _fragShader = compileShader(GL_FRAGMENT_SHADER, fragShader);
    
    if (_vertShader == kInvalid ||  _fragShader == kInvalid) {
        reset();
        return false;
    }
    
    glAttachShader(_prog, _vertShader);
    glAttachShader(_prog, _fragShader);
    glLinkProgram(_prog);
    GLint status;
    glGetProgramiv(_prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        reset();
        return false;
    }
    
    return true;
}

GLuint Program::compileShader(GLenum type, const GLchar *source) {
    GLint status;
    
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            LOG(ERROR) << "shader error " << log;
            free(log);
        }
        
        glDeleteShader(shader);
        return kInvalid;
    }

    return shader;
}

bool Program::validate() {
    GLint status;
    
    glValidateProgram(_prog);
    glGetProgramiv(_prog, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLength;
        glGetProgramiv(_prog, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(_prog, logLength, &logLength, log);
            LOG(ERROR) << "shader error " << log;
            free(log);
        }
        return false;
    }
    
    return true;
}

void Program::use() {
    if (_prog) {
        glUseProgram(_prog);
    }
}

GLuint Program::getAttribLocation(const char *name) {
    return glGetAttribLocation(_prog, name);
}

GLuint Program::getUniformLocation(const char *name) {
    return glGetUniformLocation(_prog, name);
}

const GLfloat *Program::positionVertices() {
    static const GLfloat vertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    return vertices;
}

const GLfloat *Program::texCoordForRotation(TexRotations rotation) {
    static const GLfloat noRotationTextureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat rotateLeftTextureCoordinates[] = {
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };
    
    static const GLfloat rotateRightTextureCoordinates[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    
    static const GLfloat verticalFlipTextureCoordinates[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    
    static const GLfloat horizontalFlipTextureCoordinates[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    static const GLfloat rotate180TextureCoordinates[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    
    switch(rotation)
    {
        case kTexNoRotation: return noRotationTextureCoordinates;
        case kTexRotateLeft: return rotateLeftTextureCoordinates;
        case kTexRotateRight: return rotateRightTextureCoordinates;
        case kTexFlipVertical: return verticalFlipTextureCoordinates;
        case kTexFlipHorizonal: return horizontalFlipTextureCoordinates;
        case kTexRotate180: return rotate180TextureCoordinates;
        default: return noRotationTextureCoordinates;
    }
}
