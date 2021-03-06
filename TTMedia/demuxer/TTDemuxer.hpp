//
//  TTDemuxer.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTDemuxer_hpp
#define TTDemuxer_hpp

#include <stdio.h>
#include <unistd.h>

#include "TTURL.hpp"

#include "TTPacket.hpp"


namespace TT {
    class Demuxer {
    public:
        virtual bool open(std::shared_ptr<URL> url) = 0;
        virtual void close() = 0;
        
        virtual std::shared_ptr<Packet> read() = 0;
        
        virtual bool seek(uint64_t pos) = 0;
        virtual bool isEOF() = 0;
        
        virtual bool hasAudio() = 0;
        virtual bool hasVideo() = 0;
    };
}

#endif /* TTDemuxer_hpp */
