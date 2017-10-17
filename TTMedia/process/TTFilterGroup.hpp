//
//  TTFilterGroup.hpp
//  TTPlayerExample
//
//  Created by liang on 27/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTFilterGroup_hpp
#define TTFilterGroup_hpp

#include "TTFilter.hpp"

namespace TT {
    class FilterGroup : public Filter {
    public:
        FilterGroup();
        ~FilterGroup();
        
        virtual void process(int64_t timestamp);
    };
}

#endif /* TTFilterGroup_hpp */
