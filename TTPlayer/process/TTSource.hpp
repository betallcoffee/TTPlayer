//
//  TTSource.hpp
//  TTPlayerExample
//
//  Created by liang on 13/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTSource_hpp
#define TTSource_hpp

#include <memory>
#include <vector>
#include <map>

#include "TTClass.hpp"

namespace TT {
    
    class FrameBuffer;
    class Target;
    
    class Source {
    public:
        Source();
        virtual ~Source();
        
//        void addTarget(std::shared_ptr<Target> target, int index = 0);
//        void removeTarget(std::shared_ptr<Target> target, int index = 0);
//        void removeAllTargets();
//        
//        void sendDataToTargets();
//        void notifyFrameBufferToTargets();
//        
//    protected:
//        std::shared_ptr<FrameBuffer> _frameBuffer;
//        std::map<int, std::shared_ptr<Target>> _targets;
        
        NoCopy(Source);
    };
}

#endif /* TTSource_hpp */
