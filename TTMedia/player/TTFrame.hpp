//
//  TTFrame.hpp
//  TTPlayerExample
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTFrame_hpp
#define TTFrame_hpp

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

namespace TT {
    typedef enum {
        kTextureTypeRGB,
        kTextureTypeY420p,
        kTextureTypeY420sp,
        
        kAudioPCM,
    } DataType;
    
    struct AudioDesc {
        int sampleRate;
        int channels;
        enum AVSampleFormat fmt;
        int nbSamples;
    };
    
    class Frame {
    public:
        Frame();
        Frame(AVFrame *avFrame);
        ~Frame();
        
        bool reallocData(size_t dataSize, int index);
        bool isKeyframe();
        
        enum {kNumOfPlanars = 3};
        
        DataType type;
        
        uint8_t *data[kNumOfPlanars];
        size_t lineSize[kNumOfPlanars];
        size_t numOfPlanars;
        
        size_t width;
        size_t height;
        
        int64_t pts;
        int64_t pkt_pts;
        int64_t pkt_dts;
        
        enum AVSampleFormat sampleFormat;
        
    private:
        AVFrame *_avFrame;
    };
}

#endif /* TTFrame_hpp */
