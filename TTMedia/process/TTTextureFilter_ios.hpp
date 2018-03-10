//
//  TTTextureFilter_ios.hpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTTextureFilter_ios_hpp
#define TTTextureFilter_ios_hpp

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <CoreMedia/CoreMedia.h>

#include "TTFilterTexture.hpp"

namespace TT {
    class TextureFilter_ios : public FilterTexture {
    public:
        TextureFilter_ios();
        virtual ~TextureFilter_ios();
        
        void processFrame(CMSampleBufferRef sampleBuffer);
        
    protected:
        virtual void updateTexture();
        
    protected:
        CMSampleBufferRef _sampleBuffer;
    };
}

#endif /* TTTextureFilter_ios_hpp */
