//
//  TTColorConvertFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 17/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTColorConvertFilter_hpp
#define TTColorConvertFilter_hpp

#include <OpenGLES/ES2/gl.h>

#include "TTClass.hpp"
#include "TTFilter.hpp"
#include "TTFrame.hpp"
#include "TTProgram.hpp"

namespace TT {
    class ColorConvertFilter : public Filter {
    public:
        ColorConvertFilter();
        ~ColorConvertFilter();
        
        void updateFrame(std::shared_ptr<Frame> frame);
        
    protected:
        void compileShader();
        
    private:
        void updateTexture(std::shared_ptr<Frame> frame);
        
    private:
        Program _program;
        
        GLuint _positionLocation;
        GLuint _texCoordLocation;
        
        GLuint _uniformSamplers[3];
        GLuint _uniformColorConvertionMatrix;
        
        GLuint _textures[3];
    };
}

#endif /* TTColorConvertFilter_hpp */
