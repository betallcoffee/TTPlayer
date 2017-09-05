//
//  TTNV12ToRGBFilter_ios.hpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTNV12ToRGBFilter_ios_hpp
#define TTNV12ToRGBFilter_ios_hpp

#include <CoreMedia/CoreMedia.h>

#include "TTNV12ToRGBFilter.hpp"

namespace TT {
    class NV12ToRGBFilter_ios : public NV12ToRGBFilter {
    public:
        NV12ToRGBFilter_ios();
        virtual ~NV12ToRGBFilter_ios();
        
        virtual void processFrame(CMSampleBufferRef sampleBuffer);
        
    protected:
        virtual void updateTexture();
        void updateTextureFast();
        void updateTextureRGB();
        
    private:
        CMSampleBufferRef _sampleBuffer;
        
        CVOpenGLESTextureCacheRef _videoTextureCache;
        CVOpenGLESTextureRef _lumaTexture;
        CVOpenGLESTextureRef _chromaTexture;
    };
}

#endif /* TTNV12ToRGBFilter_ios_hpp */
