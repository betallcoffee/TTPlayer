//
//  TTNV12ToRGBFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTNV12ToRGBFilter_hpp
#define TTNV12ToRGBFilter_hpp

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "TTFilterFrame.hpp"

namespace TT {
    class NV12ToRGBFilter : public FilterFrame {
    public:
        NV12ToRGBFilter();
        virtual ~NV12ToRGBFilter();
        
    protected:
        const GLchar *fragmentShader();
        void getUniformLocations();
        virtual void updateTexture();
        
    protected:
        
        GLuint _uniformSamplers[3];
        GLuint _uniformColorConvertionMatrix;
        
        GLuint _textures[3];
    };
}

#endif /* TTNV12ToRGBFilter_hpp */
