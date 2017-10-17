//
//  TTRenderYUV420p.hpp
//  TTPlayerExample
//
//  Created by liang on 16/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTRenderYUV420p_hpp
#define TTRenderYUV420p_hpp

#include "TTRender.hpp"

namespace TT {
    class ShaderYUV420p : public Shader {
    public:
        explicit ShaderYUV420p();
        
        bool uploadTexture(std::shared_ptr<Frame> frame);
        
    protected:
        bool resolveUniforms(GLuint program);
        
    private:
        int _uniformSamplers[3];
        int _uniformColorConvertionMatrix;
        
        GLuint _textures[3];
    };
}

#endif /* TTRenderYUV420p_hpp */
