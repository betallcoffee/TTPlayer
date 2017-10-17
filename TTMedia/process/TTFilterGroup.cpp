//
//  TTFilterGroup.cpp
//  TTPlayerExample
//
//  Created by liang on 27/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "TTFilterGroup.hpp"

using namespace TT;

FilterGroup::FilterGroup() {
    
}

FilterGroup::~FilterGroup() {
    
}

void FilterGroup::process(int64_t timestamp) {
    std::map<int, std::shared_ptr<Filter>>::iterator it;
    for (it = _filters.begin(); it != _filters.end(); it++) {
        // Use src framebuffer, no process
        it->second->setSrcFramebuffer(_srcFramebuffer);
        it->second->process(timestamp);
    }
}
