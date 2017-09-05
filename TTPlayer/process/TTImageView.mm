//
//  TTImageView.m
//  TTPlayerExample
//
//  Created by liang on 17/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTImageView.h"
#import "TTGLContext_ios.h"
#include "TTProgram.hpp"
#include "TTFrameBuffer.hpp"

@interface TTImageView ()
{
    std::shared_ptr<TT::Filter> _filter;
    GLuint displayRenderbuffer, displayFramebuffer;
    
    CGSize _imageSize;
    GLfloat _imageVertices[8];
    
    CGSize boundsSizeAtFrameBufferEpoch;
}

@end

@implementation TTImageView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)dealloc {
    if (_filter) {
        _filter.reset();
    }
}

- (void)layoutSubview {
    [super layoutSubviews];
    
    // The frame buffer needs to be trashed and re-created when the view size changes.
    if (!CGSizeEqualToSize(self.bounds.size, boundsSizeAtFrameBufferEpoch) &&
        !CGSizeEqualToSize(self.bounds.size, CGSizeZero)) {
            [self destroyDisplayFramebuffer];
            [self createDisplayFramebuffer];
    } else if (!CGSizeEqualToSize(self.bounds.size, CGSizeZero)) {
        [self calculateVertices];
    }
}

- (std::shared_ptr<TT::Filter>)filter {
    // TODO no thread safe
    if (_filter == nullptr) {
        std::shared_ptr<TT::Filter_ios> filter = std::make_shared<TT::Filter_ios>();
        if (filter) {
            filter->setObject(self);
        }
        _filter = filter;
    }
    return _filter;
}

- (void)setImageSize:(CGSize)size {
    if (!CGSizeEqualToSize(_imageSize, size)) {
        _imageSize = size;
        [self calculateVertices];
    }
}

- (GLfloat *)imageVertices {
    return _imageVertices;
}

- (void)calculateVertices {
    if (_imageSize.height == 0 || _imageSize.width == 0) {
        return;
    }
    
    float dH = (float)_sizeInPixels.height / _imageSize.height;
    float dW = (float)_sizeInPixels.width / _imageSize.width;
    
    switch (self.contentMode) {
        case UIViewContentModeScaleToFill:
            break;
        case UIViewContentModeScaleAspectFill:
        {
            float dd = std::max(dH, dW);
            dH = dd;
            dW = dd;
            break;
        }
        case UIViewContentModeScaleAspectFit:
        default:
        {
            float dd = std::min(dH, dW);
            dH = dd;
            dW = dd;
            break;
        }
    }
    
    const float w = (_imageSize.width  * dW / (float)_sizeInPixels.width);
    const float h = (_imageSize.height * dH / (float)_sizeInPixels.height);
    
    static const GLfloat baseVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    for (int i = 0; i < 8; i++) {
        if (i % 2) {
            _imageVertices[i] = baseVertices[i] * h;
        } else {
            _imageVertices[i] = baseVertices[i] * w;
        }
    }
}

- (void)createDisplayFramebuffer {
    TT::GLContext::sharedProcessContext().use();
    
    glGenFramebuffers(1, &displayFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
    
    glGenRenderbuffers(1, &displayRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
    
    EAGLContext *context  = (__bridge EAGLContext *)TT::GLContext::sharedProcessContext().context();
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
    
    GLint backingWidth, backingHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    
    if ( (backingWidth == 0) || (backingHeight == 0) )
    {
        [self destroyDisplayFramebuffer];
        return;
    }
    
    _sizeInPixels.width = (CGFloat)backingWidth;
    _sizeInPixels.height = (CGFloat)backingHeight;
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, displayRenderbuffer);
    
    __unused GLuint framebufferCreationStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    NSAssert(framebufferCreationStatus == GL_FRAMEBUFFER_COMPLETE, @"Failure with display framebuffer generation for display of size: %f, %f", self.bounds.size.width, self.bounds.size.height);
    boundsSizeAtFrameBufferEpoch = self.bounds.size;
    
    [self calculateVertices];
}

- (void)destroyDisplayFramebuffer;
{
    TT::GLContext::sharedProcessContext().use();
    
    if (displayFramebuffer)
    {
        glDeleteFramebuffers(1, &displayFramebuffer);
        displayFramebuffer = 0;
    }
    
    if (displayRenderbuffer)
    {
        glDeleteRenderbuffers(1, &displayRenderbuffer);
        displayRenderbuffer = 0;
    }
}

- (void)setDisplayFramebuffer;
{
    if (!displayFramebuffer)
    {
        [self createDisplayFramebuffer];
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
    glViewport(0, 0, (GLint)_sizeInPixels.width, (GLint)_sizeInPixels.height);
}

- (void)presentFramebuffer;
{
    glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
    EAGLContext *context  = (__bridge EAGLContext *)TT::GLContext::sharedProcessContext().context();
    [context presentRenderbuffer:GL_RENDERBUFFER];
}

- (BOOL)bindFramebuffer {
    [self setDisplayFramebuffer];
    CGSize size = CGSizeMake([self filter]->srcFramebuffer()->width(),
                            [self filter]->srcFramebuffer()->height());
    [self setImageSize:size];
    
    return YES;
}

- (void)draw {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    [self presentFramebuffer];
}

- (const GLfloat *)positionVertices {
    return [self imageVertices];
}

- (const GLfloat *)texCoordForRotation:(TT::TexRotations)rotation {
    static const GLfloat noRotationTextureCoordinates[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    
    static const GLfloat rotateRightTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotateLeftTextureCoordinates[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat verticalFlipTextureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat horizontalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotate180TextureCoordinates[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    switch(rotation)
    {
        case TT::kTexNoRotation: return noRotationTextureCoordinates;
        case TT::kTexRotateLeft: return rotateLeftTextureCoordinates;
        case TT::kTexRotateRight: return rotateRightTextureCoordinates;
        case TT::kTexFlipVertical: return verticalFlipTextureCoordinates;
        case TT::kTexFlipHorizonal: return horizontalFlipTextureCoordinates;
        case TT::kTexRotate180: return rotate180TextureCoordinates;
        default: return noRotationTextureCoordinates;
    }
}

@end

