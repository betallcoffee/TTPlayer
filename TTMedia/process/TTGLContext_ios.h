//
//  TTGLContext_ios.h
//  TTPlayerExample
//
//  Created by liang on 16/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTGLContext_ios_h
#define TTGLContext_ios_h

#include <OpenGLES/ES2/gl.h>

#include "TTClass.hpp"

namespace TT {
    class GLContext {
    public:
        static GLContext &sharedProcessContext();
        
        void use();
        const void *context() { return _context; }
        
    private:
        void *_context;
        
        Singleton(GLContext);
        NoCopy(GLContext);
    };
}

#endif /* TTGLContext_ios_h */
