//
//  TTFilterTexture.hpp
//  TTPlayerExample
//
//  Created by liang on 2/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTFilterTexture_hpp
#define TTFilterTexture_hpp

#include "TTFilter.hpp"

namespace TT {
    class FilterTexture : public Filter {
    public:
        FilterTexture();
        virtual ~FilterTexture();
        
        virtual void process(int64_t timestamp);
    };
}

#endif /* TTFilterTexture_hpp */
