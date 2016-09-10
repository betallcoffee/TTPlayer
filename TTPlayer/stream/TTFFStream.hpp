//
//  TTFFStream.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTFFStream_hpp
#define TTFFStream_hpp

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

#include "TTStream.hpp"

namespace TT {
    class FFStream : public Stream {
    public:
        FFStream() {};
        ~FFStream() {};
        
        bool open(std::shared_ptr<URL> url);
        void close();
        
        std::shared_ptr<Packet> read();
        bool write(std::shared_ptr<Packet> packet);
        
        bool seek(uint64_t pos);
        
        AVStream *audioStream() { return _audioStream; }
        AVStream *videoStream() { return _videoStream; }
        
    private:
        std::shared_ptr<URL> _url;
        
        AVFormatContext *_formatContext;
        AVDictionary *_option;
        
        AVStream *_audioStream;
        AVStream *_videoStream;
        
        uint8_t *buf;
    };
}

#endif /* TTFFStream_hpp */
