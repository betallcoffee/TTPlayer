//
//  TTContrastFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 18/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTContrastFilter_hpp
#define TTContrastFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class ContrastFilter : public Filter {
    public:
        ContrastFilter();
        ~ContrastFilter();
        
        void setContrast(float contrast);
        
    private:
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
    private:
        float _contrast;
        GLuint _contrastUniform;
        
        NoCopy(ContrastFilter);
    };
}

#endif /* TTContrastFilter_hpp */
