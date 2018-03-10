//
//  TTY420ToRGBFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 17/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTY420ToRGBFilter_hpp
#define TTY420ToRGBFilter_hpp

#include <OpenGLES/ES2/gl.h>

#include "TTFilterFrame.hpp"

namespace TT {
    class Y420ToRGBFilter : public FilterFrame {
    public:
        Y420ToRGBFilter();
        virtual ~Y420ToRGBFilter();
        
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

#endif /* TTY420ToRGBFilter_hpp */
