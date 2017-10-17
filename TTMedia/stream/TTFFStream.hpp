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
        FFStream() : _formatContext(nullptr), _option(nullptr),
        _audioStream(nullptr), _videoStream(nullptr) {};
        ~FFStream() {};
        
        bool open(std::shared_ptr<URL> url);
        void close();
        
        std::shared_ptr<Packet> read();
        bool write(std::shared_ptr<Packet> packet);
        
        bool seek(uint64_t pos);
        
        bool hasAudio() { return _audioStream == nullptr ? false : true; }
        bool hasVideo() { return _videoStream == nullptr ? false : true; }
        
        AVStream *audioStream() { return _audioStream; }
        AVStream *videoStream() { return _videoStream; }
        
    private:
        std::shared_ptr<URL> _url;
        
        AVFormatContext *_formatContext;
        AVDictionary *_option;
        
        AVStream *_audioStream;
        AVStream *_videoStream;
    };
}

#endif /* TTFFStream_hpp */
