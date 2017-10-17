//
//  TTQoS.hpp
//  TTPlayerExample
//
//  Created by liang on 9/9/16.
//  Copyright © 2016 tina. All rights reserved.
//

#ifndef TTQoS_hpp
#define TTQoS_hpp

#include <stdio.h>

namespace TT {
    
    class Packet;
    class Frame;
    
    class QoS {
    public:
        void addVideoPacket(const Packet *packet);
        void delVideoPacket(const Packet *packet);
        
        void addAudioPacket(const Packet *packet);
        void delAudioPacket(const Packet *packet);
        
        void addVideoFrame(const Frame *frame);
        void delVideoFrame(const Frame *frame);
        
        void addAudioFrame(const Frame *frame);
        void delAudioFrame(const Frame *frame);
        
        int bitrate();
        int rtbitrate();
        
    private:
        size_t downSize;
        size_t consumeSize;
        
    };
}

#endif /* TTQoS_hpp */
