//
//  TTFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 15/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTFilter_hpp
#define TTFilter_hpp

#include <memory>
#include <map>

#include <OpenGLES/ES2/gl.h>

#include "TTdef.h"
#include "TTClass.hpp"
#include "TTProgram.hpp"

namespace TT {
    
    class Framebuffer;
    
    class Filter {
    public:
        Filter();
        virtual ~Filter();
        
        void addFilter(std::shared_ptr<Filter> filter, int index = 0);
        void removeFilter(std::shared_ptr<Filter> filter, int index = 0);
        void removeAllFilters();

        void setSrcFramebuffer(std::shared_ptr<Framebuffer> frameBuffer);
        std::shared_ptr<Framebuffer> srcFramebuffer() { return _srcFramebuffer; }
        
        virtual void process(int64_t timestamp);
        
        virtual void notifyFramebufferToFilters(int64_t timestamp);
        
    protected:
        virtual bool bindFramebuffer();
        
        virtual void compileShader();
        virtual const GLchar *vertexShader();
        virtual void getAttribLocations();
        virtual void resolveAttribLocations();
        
        virtual const GLchar *fragmentShader();
        virtual void getUniformLocations();
        virtual void resolveUniformLocations();
        
        virtual const GLfloat *positionVertices();
        virtual const GLfloat *texCoordForRotation(TexRotations rotation);
        
        virtual void updateTexture();
        
        virtual void draw();
        
    protected:
        std::shared_ptr<Framebuffer> _srcFramebuffer;
        std::shared_ptr<Framebuffer> _framebuffer;
        std::map<int, std::shared_ptr<Filter>> _filters;
        
        Program _program;
        
        GLuint _positionLocation;
        GLuint _texCoordLocation;
        GLuint _textureUniform;
        
        NoCopy(Filter);
        
    };
}

#endif /* TTFilter_hpp */
