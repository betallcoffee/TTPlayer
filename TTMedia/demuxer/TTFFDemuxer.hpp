//
//  TTFFDemuxer.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTFFDemuxer_hpp
#define TTFFDemuxer_hpp

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

#include "TTDemuxer.hpp"

namespace TT {
    class FFDemuxer : public Demuxer {
    public:
        FFDemuxer() : _formatContext(nullptr), _option(nullptr),
        _audioStream(nullptr), _videoStream(nullptr) {};
        ~FFDemuxer() {};
        
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

#endif /* TTFFDemuxer_hpp */
