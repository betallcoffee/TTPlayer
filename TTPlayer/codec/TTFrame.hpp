//
//  TTFrame.hpp
//  TTPlayerExample
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTFrame_hpp
#define TTFrame_hpp

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

namespace TT {
    typedef enum {
        kFrameTypeRGB,
        kFrameTypeY420p,
        kFrameTypeY420sp
    }eFrameType;
    
    class Frame {
    public:
        Frame(AVFrame *avFrame);
        ~Frame();
        
        static const size_t kNumOfPlanars = 3;
        
        eFrameType type;
        
        uint8_t *data[kNumOfPlanars];
        size_t lineSize[kNumOfPlanars];
        size_t numOfPlanars;
        
        size_t width;
        size_t height;
        
        int64_t pts;
        int64_t pkt_pts;
        int64_t pkt_dts;
        
    private:
        AVFrame *_avFrame;
    };
}

#endif /* TTFrame_hpp */
