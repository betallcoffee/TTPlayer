//
//  TTFFMuxer.hpp
//  TTPlayerExample
//
//  Created by liang on 19/2/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTFFMuxer_hpp
#define TTFFMuxer_hpp

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

#include "TTMuxer.hpp"

namespace TT {
    class FFMuxer : public Muxer {
    public:
        FFMuxer();
        ~FFMuxer();
        
        bool open(std::shared_ptr<URL> url, AVCodecParameters *audioCodecParam, AVCodecParameters *videoCodecParam) override;
        void close() override;
        bool write(std::shared_ptr<Packet> packet) override;
        
    private:
        std::shared_ptr<URL> _url = nullptr;
        
        pthread_mutex_t _mutex;
        
        AVFormatContext *_formatContext = nullptr;
        AVDictionary *_option = nullptr;
        
        AVStream *_audioStream = nullptr;
        AVStream *_videoStream = nullptr;
    };
}

#endif /* TTFFMuxer_hpp */
