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
    TT::Framebuffer *_framebuffer;
    GLuint displayRenderbuffer, displayFramebuffer;
    
    CGSize imageSize;
    GLfloat imageVertices[8];
    
    CGSize boundsSizeAtFrameBufferEpoch;
}

@end

@implementation TTImageView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)dealloc {
    
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

- (void)setImageSize:(CGSize)size {
    if (!CGSizeEqualToSize(imageSize, size)) {
        imageSize = size;
        [self calculateVertices];
    }
}

- (CGFloat *)imageVertices {
    return imageVertices;
}

- (void)calculateVertices {
    if (imageSize.height == 0 || imageSize.width == 0) {
        return;
    }
    
    float dH = (float)_sizeInPixels.height / imageSize.height;
    float dW = (float)_sizeInPixels.width / imageSize.width;
    
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
    
    const float w = (imageSize.width  * dW / (float)_sizeInPixels.width);
    const float h = (imageSize.height * dH / (float)_sizeInPixels.height);
    
    static const GLfloat baseVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    for (int i = 0; i < 8; i++) {
        if (i % 2) {
            imageVertices[i] = baseVertices[i] * h;
        } else {
            imageVertices[i] = baseVertices[i] * w;
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

@end

TT::ImageView::ImageView() {
    _imageView = (__bridge_retained void *)[TTImageView new];
}

TT::ImageView::~ImageView() {
    if (_imageView) {
        CFBridgingRelease(_imageView);
    }
    _imageView = nullptr;
}

//void TT::ImageView::process() {
//    if (_srcFramebuffer) {
//        GLContext::sharedProcessContext().use();
//        
//        
//        if (!_program.isCompiled())
//            compileShader();
//        _program.use();
//        
//        
//        
//        glClearColor(0, 0, 0, 0);
//        glClear(GL_COLOR_BUFFER_BIT);
//        
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, _srcFramebuffer->textrue());
//        
//        glUniform1i(_textureUniform, 1);
//        
//        resolveAttribLocations();
//        
//        
//    }
//}

bool TT::ImageView::bindFramebuffer() {
    TTImageView *imageView = (__bridge TTImageView *)_imageView;
    [imageView setDisplayFramebuffer];
    
    CGSize imageSize = CGSizeMake(_srcFramebuffer->width(), _srcFramebuffer->height());
    [imageView setImageSize:imageSize];
    
    return true;
}

void TT::ImageView::draw() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    TTImageView *imageView = (__bridge TTImageView *)_imageView;
    [imageView presentFramebuffer];
}

const GLfloat *TT::ImageView::positionVertices() {
    return [(__bridge TTImageView *)_imageView imageVertices];
}

const GLfloat *TT::ImageView::texCoordForRotation(TexRotations rotation) {
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
        case kTexNoRotation: return noRotationTextureCoordinates;
        case kTexRotateLeft: return rotateLeftTextureCoordinates;
        case kTexRotateRight: return rotateRightTextureCoordinates;
        case kTexFlipVertical: return verticalFlipTextureCoordinates;
        case kTexFlipHorizonal: return horizontalFlipTextureCoordinates;
        case kTexRotate180: return rotate180TextureCoordinates;
        default: return noRotationTextureCoordinates;
    }
}

