//
//  TTPacket.cpp
//  TTPlayerExample
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "TTPacket.hpp"

using namespace TT;

Packet::Packet(AVPacket *avpacket) :_avpacket(avpacket) {
    if (_avpacket) {
        pts = _avpacket->pts;
        dts = _avpacket->dts;
        pos = _avpacket->pos;
    }
}

Packet::~Packet() {
    if (_avpacket) {
        av_packet_unref(_avpacket);
        av_free(_avpacket);
        _avpacket = NULL;
    }
}
