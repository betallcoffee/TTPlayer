//
//  TTStream.hpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTStream_hpp
#define TTStream_hpp

#include <stdio.h>
#include <unistd.h>

#include "TTURL.hpp"

#include "TTPacket.hpp"


namespace TT {
    class Stream {
    public:
        virtual bool open(std::shared_ptr<URL> url) = 0;
        virtual void close() = 0;
        
        virtual std::shared_ptr<Packet> read() = 0;
        virtual bool write(std::shared_ptr<Packet> packet) = 0;
        
        virtual bool seek(uint64_t pos) = 0;
        
        virtual bool hasAudio() = 0;
        virtual bool hasVideo() = 0;
    };
}

#endif /* TTStream_hpp */
