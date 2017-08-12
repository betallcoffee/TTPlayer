//
//  TTAudioCodec.hpp
//  TTPlayerExample
//
//  Created by liang on 10/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTAudioCodec_hpp
#define TTAudioCodec_hpp

#include <stdio.h>
#include <string>
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
};
#endif

#include "TTFrame.hpp"
#include "TTPacket.hpp"

namespace TT {
    
    typedef void (AudioCodecCB)(AudioDesc &desc);
    
    class AudioCodec {
    public:
        AudioCodec(AVStream *avStream);
        ~AudioCodec();
        
        bool open();
        void close();
        std::shared_ptr<Frame> decode(std::shared_ptr<Packet> packet);
        
        void setCodecCallback(std::function<AudioCodecCB> cb) { _cb = cb; }
        
    private:
        void createSwrContext();
        
    private:
        AVStream *_avStream;
        AVCodecContext *_avCodecContext;
        AVCodec *_avCodec;
        
        AVFrame *_avFrame;
        
        SwrContext *_swrContext;
        
        int _sampleRate;
        int64_t _channelLayout;
        enum AVSampleFormat _fmt;
        int _nbSamples;
        
        std::function<AudioCodecCB> _cb;
    };
}

#endif /* TTAudioCodec_hpp */
