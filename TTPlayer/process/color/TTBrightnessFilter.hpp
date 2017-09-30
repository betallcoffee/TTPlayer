//
//  TTBrightnessFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 29/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTBrightnessFilter_hpp
#define TTBrightnessFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class BrightnessFilter : public Filter {
    public:
        BrightnessFilter();
        ~BrightnessFilter();
        
        // Brightness ranges from -1.0 to 1.0, with 0.0 as the normal level
        void setBrightness(float brightness);
        
    private:
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    private:
        float _brightness;
        GLuint _brightnessUniform;
        
        NoCopy(BrightnessFilter);
    };
}

#endif /* TTBrightnessFilter_hpp */
