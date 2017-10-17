//
//  TTTextureFilter.hpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTTextureFilter_hpp
#define TTTextureFilter_hpp

#include "TTFilter.hpp"

namespace TT {
    class TextureFilter : public Filter {
    public:
        TextureFilter();
        virtual ~TextureFilter();
        
        virtual void process(int64_t timestamp);
    };
}

#endif /* TTTextureFilter_hpp */
