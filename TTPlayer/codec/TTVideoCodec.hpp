//
//  TTVideoCodec.hpp
//  TTPlayerExample
//
//  Created by liang on 9/9/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTVideoCodec_hpp
#define TTVideoCodec_hpp

#include <stdio.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

#include "TTFrame.hpp"
#include "TTPacket.hpp"

namespace TT {
    class VideoCodec {
    public:
        VideoCodec(AVStream *avStream);
        ~VideoCodec();
        
        bool open();
        void close();
        std::shared_ptr<Frame> decode(std::shared_ptr<Packet> packet);
        
    private:
        AVStream *_avStream;
        AVCodecContext *_avCodecContext;
        AVCodec *_avCodec;
    };
}

#endif /* TTVideoCodec_hpp */
