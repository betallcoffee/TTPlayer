//
//  TTMuxer.hpp
//  TTPlayerExample
//
//  Created by liang on 19/2/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTMuxer_hpp
#define TTMuxer_hpp

#include <stdio.h>
#include <unistd.h>

#include "TTURL.hpp"

#include "TTPacket.hpp"


namespace TT {
    class Muxer {
    public:
        virtual bool open(std::shared_ptr<URL> url, AVCodecParameters *audioCodecParam, AVCodecParameters *videoCodecParam) = 0;
        virtual void close() = 0;
        virtual bool write(std::shared_ptr<Packet> packet) = 0;
    };
}

#endif /* TTMuxer_hpp */
