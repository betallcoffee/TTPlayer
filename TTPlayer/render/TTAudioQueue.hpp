//
//  TTAudioQueue.hpp
//  TTPlayerExample
//
//  Created by liang on 10/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTAudioQueue_hpp
#define TTAudioQueue_hpp

#include <stdio.h>
#include <stdint.h>
#include <memory>
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/samplefmt.h"
#ifdef __cplusplus
};
#endif

#include "TTFrame.hpp"

namespace TT {
    
    typedef std::shared_ptr<Frame> (QueueCallback)();

    class AudioQueue {
    public:
        AudioQueue() {}
        virtual ~AudioQueue() { stop(); teardown(); }
        
        virtual bool setup(AudioDesc &desc) = 0;
        virtual void teardown() {};
        
        virtual void start() = 0;
        virtual void stop() {};
        
        virtual void pause() = 0;
        virtual void resume() = 0;
        
        void setQueueCallback(std::function<QueueCallback> cb) {
            _cb = cb;
        }
        
    protected:
        std::function<QueueCallback> _cb;
        
    };
}

#endif /* TTAudioQueue_hpp */
