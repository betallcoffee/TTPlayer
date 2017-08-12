//
//  TTRender.cpp
//  TTPlayerExample
//
//  Created by liang on 9/12/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <assert.h>
#include <stdlib.h>
#include <algorithm>

#include "easylogging++.h"###

#include "TTRender.hpp"
#include "TTShaderYUV420p.hpp"

using namespace TT;

const static GLint kInvalid = 0;

static const GLchar *const vertexShaderString = STRINGIZE
(
 attribute vec4 position;
 attribute vec2 texcoord;
 uniform mat4 modelViewProjectionMatrix;
 varying vec2 v_texcoord;
 
 void main()
 {
     gl_Position = position;
     v_texcoord = texcoord.xy;
 }
);

Shader::Shader() : _type(kTextureTypeRGB),
 _vertexShader(vertexShaderString), _fragmentShader(nullptr),
 _vertexLocation(-1), _texCoordLocation(-1) {
    
}

bool Shader::resolveUniforms(GLuint program) {
    if (program != kInvalid) {
        _vertexLocation = glGetAttribLocation(program, "position");
        _texCoordLocation = glGetAttribLocation(program, "texcoord");
        return true;
    }
    return false;
}

Render::Render() : _framebuffer(kInvalid),
 _renderbuffer(kInvalid),
 _backingWidth(kInvalid),
 _backingHeight(kInvalid),
 _program(kInvalid), _shader(nullptr),
 _contentMode(kContentModeScaleAspectFit), _rotationMode(kNoRotation){
    
}

void Render::bindContext(const RenderContext &context) {
    _renderCtx.opaque = context.opaque;
    _renderCtx.setup = context.setup;
    _renderCtx.teardown = context.teardown;
    _renderCtx.display = context.display;
}

bool Render::displayFrame(std::shared_ptr<Frame> frame) {
    if (_renderCtx.display == nullptr ||
        _renderCtx.setup == nullptr) {
        return false;
    }
    
    _textureType = frame->type;
    if (_renderCtx.setup(_renderCtx.opaque, this)) {
        return _renderCtx.display(_renderCtx.opaque, frame);
    }
    
    return false;
}

bool Render::createFrameBuffer() {
    if (_framebuffer != kInvalid) {
        return true;
    }
    
    glGenFramebuffers(1, &_framebuffer);
    
    return true;
}

bool Render::bindFrameBuffer() {
    if (_framebuffer == kInvalid) {
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    
    return true;
}

bool Render::createRenderBuffer() {
    if (_renderbuffer != kInvalid ) {
        return true;
    }
    
    glGenRenderbuffers(1, &_renderbuffer);
    
    return true;
}

bool Render::bindRenderBuffer() {
    if (_renderbuffer == kInvalid) {
        return false;
    }
    
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    
    return true;
}

bool Render::attachRenderBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backingHeight);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderbuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }
    
    GLenum glError = glGetError();
    if (GL_NO_ERROR != glError) {
        return false;
    }

    return true;
}

bool Render::loadShader() {
    bool result = false;
    
    if (_shader == nullptr || _shader->type() != _textureType) {
        switch (_textureType) {
            case kTextureTypeY420p:
                _shader = std::make_shared<ShaderYUV420p>();
                break;
                
            default:
                return false;
        }
    }
    
    GLuint vertShader = 0, fragShader = 0;
    
    if (_program > kInvalid) {
        glDeleteProgram(_program);
        _program = kInvalid;
    }
    
    vertShader = compileShader(GL_VERTEX_SHADER, _shader->vertexShader());
    if (!vertShader)
        goto exit;
    
    fragShader = compileShader(GL_FRAGMENT_SHADER, _shader->fragmentShader());
    if (!fragShader)
        goto exit;
    
    _program = glCreateProgram();
    
    glAttachShader(_program, vertShader);
    glAttachShader(_program, fragShader);
    glLinkProgram(_program);
    
    GLint status;
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        goto exit;
    }
    
    result = validateProgram(_program);
    
    if (result) {
        _shader->resolveUniforms(_program);
    }
    
exit:
    
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    if (!result){
        glDeleteProgram(_program);
        _program = kInvalid;
    }
    
    return result;
}

GLuint Render::compileShader(GLenum type, const GLchar *sources) {
    
    GLuint shader = glCreateShader(type);
    if (shader == kInvalid || shader == GL_INVALID_ENUM) {
        return kInvalid;
    }
    
    glShaderSource(shader, 1, &sources, nullptr);
    glCompileShader(shader);
    
#ifdef DEBUG
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        free(log);
    }
#endif
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glDeleteShader(shader);
        return kInvalid;
    }
    
    return shader;
}

bool Render::validateProgram(GLuint prog)
{
    GLint status;
    
    glValidateProgram(prog);
    
#ifdef DEBUG
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        return false;
    }
    
    return true;
}

bool Render::createProjectMatrix() {
    return false;
}

bool Render::createModelViewMatrix() {
    return false;
}

bool Render::bindMatrix() {
    return false;
}

bool Render::updateBuffers(int sarNum, int sarDen,
                           float width, float height) {
    printf("updateBuffers: width(%.2f), height(%.2f)\n", width, height);
    
    updateVerticesBuffers(sarNum, sarDen, width, height);
    updateTexCoordsBuffers(width, height);
    updateIndicesBuffers();
    
    return true;
}

void Render::updateVerticesBuffers(int sarNum, int sarDen, float width, float height) {
    if (_rotationMode == kRotateRight ||
        _rotationMode == kRotateLeft) {
        float temp = width;
        width = height;
        height = temp;
    }
    if (sarNum > 0 && sarDen > 0) {
        width = width * sarNum / sarDen;
    }
    
    GLfloat vertices[12];
    GLsizeiptr verticesSize = sizeof(vertices);
    
    float dH = (float)_backingHeight / height;
    float dW = (float)_backingWidth / width;
    switch (_contentMode) {
        case kContentModeScaleToFill:
            break;
        case kContentModeScaleAspectFill:
        {
            float dd = std::max(dH, dW);
            dH = dd;
            dW = dd;
            break;
        }
        default:
        {
            float dd = std::min(dH, dW);
            dH = dd;
            dW = dd;
            break;
        }
    }
    
    const float w = (width  * dW / (float)_backingWidth);
    const float h = (height * dH / (float)_backingHeight);
    
    vertices[0] = - w;
    vertices[1] = - h;
    vertices[2] = 0;
    vertices[3] = w;
    vertices[4] = - h;
    vertices[5] = 0;
    vertices[6] = - w;
    vertices[7] = h;
    vertices[8] = 0;
    vertices[9] = w;
    vertices[10] = h;
    vertices[11] = 0;
    
    if (_verticesBuffer) {
        glDeleteBuffers(1, &_verticesBuffer);
        _verticesBuffer = 0;
    }
    
    glGenBuffers(1, &_verticesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(_shader->vertexLocation(), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(_shader->vertexLocation());
}

void Render::updateTexCoordsBuffers(float width, float heigth) {
    
    float w = 1.0f;
    // videotoolbox 解码获得的 CVPixelBufferRef 的 PerRowOfPlane 值不一定与 frame width 一致。在右边出现绿边
    // 解决办法：在创建 shader 中的 texcoord 时调整纹理右边的坐标，达到裁剪右边出现的绿边。
    // 由于精度问题，多裁剪一个像素。
    //        if (planeWidth - frameSize.width > 0.01) {
    //            w = (frameSize.width - 1) / planeWidth;
    //        }
    
    GLfloat texCoords[8];
    GLsizeiptr texCoordSize = sizeof(texCoords);
    
    switch(_rotationMode)
    {
        case kRotateLeft:
            static GLfloat rotateLeftTextureCoordinates[] = {
                1.0f, 1.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                0.0f, 0.0f,
            };
            rotateLeftTextureCoordinates[0] = w;
            rotateLeftTextureCoordinates[2] = w;
            memcpy(texCoords, rotateLeftTextureCoordinates, sizeof(GLfloat) * 8);
            break;
        case kRotateRight:
            static GLfloat rotateRightTextureCoordinates[] = {
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,
            };
            rotateRightTextureCoordinates[4] = w;
            rotateRightTextureCoordinates[6] = w;
            memcpy(texCoords, rotateRightTextureCoordinates, sizeof(GLfloat) * 8);
            break;
        case kFlipVertical:
            static GLfloat verticalFlipTextureCoordinates[] = {
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
            };
            verticalFlipTextureCoordinates[2] = w;
            verticalFlipTextureCoordinates[6] = w;
            memcpy(texCoords, verticalFlipTextureCoordinates, sizeof(GLfloat) * 8);
            break;
        case kFlipHorizonal:
            static GLfloat horizontalFlipTextureCoordinates[] = {
                1.0f, 1.0f,
                0.0f, 1.0f,
                1.0f,  0.0f,
                0.0f,  0.0f,
            };
            horizontalFlipTextureCoordinates[0] = w;
            horizontalFlipTextureCoordinates[4] = w;
            memcpy(texCoords, horizontalFlipTextureCoordinates, sizeof(GLfloat) * 8);
            break;
        case kRotate180:
            static GLfloat rotate180TextureCoordinates[] = {
                1.0f, 0.0f,
                0.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f,
            };
            rotate180TextureCoordinates[0] = w;
            rotate180TextureCoordinates[4] = w;
            memcpy(texCoords, rotate180TextureCoordinates, sizeof(GLfloat) * 8);
            break;
        default:
            static GLfloat noRotationTextureCoordinates[] = {
                0.0f, 1.0f,
                1.0f, 1.0f,
                0.0f,  0.0f,
                1.0f,  0.0f,
            };
            noRotationTextureCoordinates[2] = w;
            noRotationTextureCoordinates[6] = w;
            memcpy(texCoords, noRotationTextureCoordinates, sizeof(GLfloat) * 8);
            break;
    }
    
    if (_texCoordsBuffer) {
        glDeleteBuffers(1, &_texCoordsBuffer);
        _texCoordsBuffer = 0;
    }
    
    glGenBuffers(1, &_texCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoordSize, texCoords, GL_STATIC_DRAW);
    
    glVertexAttribPointer(_shader->texCoordLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(_shader->texCoordLocation());
}

void Render::updateIndicesBuffers() {
    GLuint indices[6] = {0, 1, 2, 2, 1, 3};
    GLsizeiptr indicesSize = sizeof(indices);
    _indicesCount = 6;
    
    if (_indicesBuffer) {
        glDeleteBuffers(1, &_indicesBuffer);
        _indicesBuffer = 0;
    }
    
    glGenBuffers(1, &_indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
}

bool Render::uploadTexture(std::shared_ptr<Frame> frame) {
    TIMED_FUNC(timer);
    if (_shader == nullptr) {
        return false;
    } else {
        bindFrameBuffer();
        bindRenderBuffer();
        
        glUseProgram(_program);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, _backingWidth, _backingHeight);
        
        PERFORMANCE_CHECKPOINT(timer);
        bool ret = _shader->uploadTexture(frame);
        PERFORMANCE_CHECKPOINT(timer);
        if (ret) {
            glDrawElements(GL_TRIANGLES, _indicesCount, GL_UNSIGNED_INT, 0);
            PERFORMANCE_CHECKPOINT(timer);
        }
        
        return ret;
    }
}

