//
//  TTFramebuffer.hpp
//  TTPlayerExample
//
//  Created by liang on 14/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTFramebuffer_hpp
#define TTFramebuffer_hpp

#include <stdint.h>
#include <stddef.h>
#include <OpenGLES/ES2/gl.h>

namespace TT {
    class Framebuffer {
    public:
        Framebuffer();
        ~Framebuffer();
        
        bool setUp(size_t width, size_t height);
        void tearDown();
        
        void active();
        void *newImage;
        
        size_t width() const { return _width; }
        size_t height() const { return _height; }
        GLuint textrue() const { return _texture; }
        
    private:
        size_t _width;
        size_t _height;
        GLuint _framebuffer;
        GLuint _texture;
    };
}

#endif /* TTFramebuffer_hpp */
