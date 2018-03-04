//
//  TTPacket.hpp
//  TTPlayerExample
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTPacket_hpp
#define TTPacket_hpp

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
};
#endif

namespace TT {
    typedef enum {
        kPacketTypeNone,
        kPacketTypeAudio,
        kPacketTypeVideo
    }ePacketType;
    
    class Packet {
    public:
        Packet(AVPacket *avpacket);
        ~Packet();
        
        const uint8_t *data() { return _avpacket->data; }
        size_t size() { return _avpacket->size; }
        
        ePacketType type;
        int64_t dts;
        int64_t pts;
        int64_t pos;
        
        AVPacket *avpacket() { return _avpacket; }
        
    private:
        AVPacket *_avpacket;
    };
}

#endif /* TTPacket_hpp */
