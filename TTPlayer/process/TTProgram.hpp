//
//  TTProgram.hpp
//  TTPlayerExample
//
//  Created by liang on 16/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTProgram_hpp
#define TTProgram_hpp

#include <OpenGLES/ES2/gl.h>

#include "TTClass.hpp"

namespace TT {
    typedef enum {
        kTexNoRotation, // 无旋转
        kTexRotateLeft, // 向左旋
        kTexRotateRight, // 向右旋
        kTexFlipVertical, // 垂直翻转
        kTexFlipHorizonal, // 水平翻转
        kTexRotate180 // 旋转 180 度
    } TexRotations;
    
    class Program {
    public:
        explicit Program();
        ~Program();
        
        bool isCompiled();
        bool compile(const GLchar *vertShader, const GLchar *fragShader);
        bool validate();
        void use();
        
        GLuint getAttribLocation(const char *name);
        GLuint getUniformLocation(const char *name);
        
        static const GLfloat *positionVertices();
        static const GLfloat *texCoordForRotation(TexRotations rotation);
        
    private:
        void reset();
        GLuint compileShader(GLenum type, const GLchar *source);
        
    private:
        GLuint _prog;
        GLuint _vertShader;
        GLuint _fragShader;
        
        NoCopy(Program);
    };
}

#endif /* TTProgram_hpp */
