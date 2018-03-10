//
//  TTFilterFrame.hpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTFilterFrame_hpp
#define TTFilterFrame_hpp

#include "TTFrame.hpp"
#include "TTPacket.hpp"
#include "TTFilter.hpp"

namespace TT {
    class FilterFrame : public Filter {
    public:
        FilterFrame();
        virtual ~FilterFrame();
        
        virtual void processFrame(std::shared_ptr<Frame> frame);
        
    protected:
        std::shared_ptr<Frame> _frame;
    };
}

#endif /* TTFilterFrame_hpp */
