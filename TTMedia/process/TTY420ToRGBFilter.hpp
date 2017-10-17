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

#include "TTFilter.hpp"
#include "TTFrame.hpp"

namespace TT {
    class Y420ToRGBFilter : public Filter {
    public:
        Y420ToRGBFilter();
        virtual ~Y420ToRGBFilter();
        
        virtual void processFrame(std::shared_ptr<Frame> frame);
        
    protected:
        const GLchar *fragmentShader();
        void getUniformLocations();
        virtual void updateTexture();
        
    protected:
        std::shared_ptr<Frame> _frame;
      
        GLuint _uniformSamplers[3];
        GLuint _uniformColorConvertionMatrix;
        
        GLuint _textures[3];
    };
}

#endif /* TTY420ToRGBFilter_hpp */
