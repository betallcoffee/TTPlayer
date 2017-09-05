//
//  TTTextureFilter_ios.cpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTTextureFilter_ios.hpp"
#include "TTProcessPriv.h"
#include "TTGLContext_ios.h"
#include "TTFramebuffer.hpp"

using namespace TT;

TextureFilter_ios::TextureFilter_ios() {
}

TextureFilter_ios::~TextureFilter_ios() {
    
}

void TextureFilter_ios::processFrame(CMSampleBufferRef sampleBuffer) {
    if (sampleBuffer) {
        _sampleBuffer = sampleBuffer;
        CVImageBufferRef cameraFrame = CMSampleBufferGetImageBuffer(sampleBuffer);
        _width = CVPixelBufferGetWidth(cameraFrame);
        _height = CVPixelBufferGetHeight(cameraFrame);
        CMTime timestamp = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
        int64_t pts = CMTimeGetSeconds(timestamp) * 1000;
        process(pts);
        _sampleBuffer = nullptr;
    }
}

void TextureFilter_ios::updateTexture() {
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(_sampleBuffer);
    
    int pixelFormatType = CVPixelBufferGetPixelFormatType(pixelBuffer);
    if (pixelFormatType != kCVPixelFormatType_32BGRA) {
        LOG(WARNING) << "The pixel format type " << pixelFormatType << " not support";
        return;
    }
    
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _framebuffer->textrue());
    
    // Using BGRA extension to pull in video frame data directly
    // The use of bytesPerRow / 4 accounts for a display glitch present in preview video frames when using the photo preset on the camera
    int bytesPerRow = (int) CVPixelBufferGetBytesPerRow(pixelBuffer);
    const void *bytes = CVPixelBufferGetBaseAddress(pixelBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bytesPerRow / 4, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bytes);
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
}
