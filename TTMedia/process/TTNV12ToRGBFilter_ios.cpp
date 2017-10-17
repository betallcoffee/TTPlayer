//
//  TTNV12ToRGBFilter_ios.cpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTNV12ToRGBFilter_ios.hpp"
#include "TTProcessPriv.h"
#include "TTGLContext_ios.h"

using namespace TT;

NV12ToRGBFilter_ios::NV12ToRGBFilter_ios() : _lumaTexture(nullptr), _chromaTexture(nullptr) {
    CVReturn err;
    CVEAGLContext context = (CVEAGLContext)GLContext::sharedProcessContext().context();
    err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, nullptr,
                                       context, nullptr,
                                       &_videoTextureCache);
    if (err != noErr) {
        LOG(ERROR) << "Error at CVOpenGLESTextureCacheCreate " << err;
    }
}

NV12ToRGBFilter_ios::~NV12ToRGBFilter_ios() {
    if (_videoTextureCache) {
        CFRelease(_videoTextureCache);
        _videoTextureCache = nullptr;
    }
}

void NV12ToRGBFilter_ios::processFrame(CMSampleBufferRef sampleBuffer) {
    if (sampleBuffer) {
        _sampleBuffer = sampleBuffer;
        CVImageBufferRef cameraFrame = CMSampleBufferGetImageBuffer(sampleBuffer);
        _width = CVPixelBufferGetWidth(cameraFrame);
        _height = CVPixelBufferGetWidth(cameraFrame);
        CMTime timestamp = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
        int64_t pts = CMTimeGetSeconds(timestamp) * 1000;
        process(pts);
        _sampleBuffer = nullptr;
    }
}

void NV12ToRGBFilter_ios::updateTexture() {
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(_sampleBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    
    if (0 == _textures[0])
        glGenTextures(2, _textures);
    
    glActiveTexture(GL_TEXTURE0);
    
    glBindTexture(GL_TEXTURE_2D, _textures[0]);
    const size_t lumaStride = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    const void *luma = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED_EXT,
                 lumaStride,
                 _height,
                 0,
                 GL_RED_EXT,
                 GL_UNSIGNED_BYTE,
                 luma);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textures[0]);
    glUniform1i(_uniformSamplers[0], 0);
    
    size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer);
    if(planeCount == 2) {
        // UV-plane.
        glActiveTexture(GL_TEXTURE1);
        
        glBindTexture(GL_TEXTURE_2D, _textures[1]);
        const size_t chromaStride = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
        const void *chromaB = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RG_EXT,
                     chromaStride,
                     _height / 2,
                     0,
                     GL_RG_EXT,
                     GL_UNSIGNED_BYTE,
                     chromaB);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(_uniformSamplers[1], 1);
    }
    
    glUniformMatrix3fv(_uniformColorConvertionMatrix, 1, GL_FALSE, kColorConversionMatrix601);
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
}

void NV12ToRGBFilter_ios::updateTextureFast() {
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(_sampleBuffer);
    
    /*
     CVOpenGLESTextureCacheCreateTextureFromImage will create GLES texture optimally from CVPixelBufferRef.
     */
    
    /*
     Create Y and UV textures from the pixel buffer. These textures will be drawn on the frame buffer Y-plane.
     */
    
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                       _videoTextureCache,
                                                       pixelBuffer,
                                                       NULL,
                                                       GL_TEXTURE_2D,
                                                       GL_RED_EXT,
                                                       _width,
                                                       _height,
                                                       GL_RED_EXT,
                                                       GL_UNSIGNED_BYTE,
                                                       0,
                                                       &_lumaTexture);
    if (err) {
        LOG(ERROR) << "Error at CVOpenGLESTextureCacheCreateTextureFromImage " << err;
    }
    
    _textures[0] = CVOpenGLESTextureGetName(_lumaTexture);
    glBindTexture(CVOpenGLESTextureGetTarget(_lumaTexture), CVOpenGLESTextureGetName(_lumaTexture));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textures[0]);
    glUniform1i(_uniformSamplers[0], 0);
    
    size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer);
    if(planeCount == 2) {
        // UV-plane.
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                           _videoTextureCache,
                                                           pixelBuffer,
                                                           NULL,
                                                           GL_TEXTURE_2D,
                                                           GL_RG_EXT,
                                                           _width / 2,
                                                           _height / 2,
                                                           GL_RG_EXT,
                                                           GL_UNSIGNED_BYTE,
                                                           1,
                                                           &_chromaTexture);
        if (err) {
            LOG(ERROR) << "Error at CVOpenGLESTextureCacheCreateTextureFromImage " << err;
        }
        
        _textures[1] = CVOpenGLESTextureGetName(_chromaTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(CVOpenGLESTextureGetTarget(_chromaTexture), CVOpenGLESTextureGetName(_chromaTexture));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(_uniformSamplers[1], 1);
    }
    
    glUniformMatrix3fv(_uniformColorConvertionMatrix, 1, GL_FALSE, kColorConversionMatrix601);
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
}
