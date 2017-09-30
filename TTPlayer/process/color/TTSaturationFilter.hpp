//
//  TTSaturationFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 30/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTSaturationFilter_hpp
#define TTSaturationFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class SaturationFilter : public Filter {
    public:
        SaturationFilter();
        ~SaturationFilter();
        
        /** Saturation ranges from 0.0 (fully desaturated) to 2.0 (max saturation), with 1.0 as the normal level
         */
        void setSaturation(float saturation);
        
    private:
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    private:
        float _saturation;
        GLint _saturationUniform;
        
        NoCopy(SaturationFilter);
    };
}

#endif /* TTSaturationFilter_hpp */
