//
//  TTFFDemuxer.cpp
//  TTPlayer
//
//  Created by liang on 6/11/16.
//  Copyright © 2016 tina. All rights reserved.
//

#include "TTMutex.hpp"
#include "TTFFDemuxer.hpp"

using namespace TT;

bool FFDemuxer::open(std::shared_ptr<URL> url) {
    _url = url;
    
    _formatContext = avformat_alloc_context();
//    av_dict_set_int(&_option, "probesize", 100*1024, 0);
    
//    _formatContext->interrupt_callback.callback = read_interrupt_cb;
//    _formatContext->interrupt_callback.opaque = (__bridge void *)(self);
    
    int ret;
    if((ret = avformat_open_input(&_formatContext, _url->cStr(), NULL, NULL)) != 0) {
        printf("Couldn't open input stream %d %s.\n", ret, av_err2str(ret));
        return false;
    }
    
    av_format_inject_global_side_data(_formatContext);
    
    if((ret = avformat_find_stream_info(_formatContext,NULL))<0){
        printf("Couldn't find stream information %d.\n", ret);
        return false;
    }
    
    av_dump_format(_formatContext, 0, _url->cStr(), 0);
    
    _audioStream = _videoStream = nullptr;
    for(int i=0; i<_formatContext->nb_streams; i++){
        int type = _formatContext->streams[i]->codec->codec_type;
        if (type == AVMEDIA_TYPE_AUDIO) {
            _audioStream = _formatContext->streams[i];
        } else if (type == AVMEDIA_TYPE_VIDEO) {
            _videoStream = _formatContext->streams[i];
        }
    }
    
    if (_audioStream == nullptr &&
        _videoStream == nullptr) {
        return false;
    }
    
    return true;
}

void FFDemuxer::close() {
    if (_formatContext) {
        avformat_close_input(&_formatContext);
    }
}

std::shared_ptr<Packet> FFDemuxer::read() {
    Mutex m(&_mutex);
    if (_formatContext == nullptr) {
        return nullptr;
    }
    
    AVPacket *avpacket=(AVPacket *)av_malloc(sizeof(AVPacket));
    int ret = av_read_frame(_formatContext, avpacket);
    if (ret == 0) {
        std::shared_ptr<Packet> packet = std::make_shared<Packet>(avpacket);
        if (avpacket->stream_index == _audioStream->index) {
            packet->type = kPacketTypeAudio;
        } else if (avpacket->stream_index == _videoStream->index) {
            packet->type = kPacketTypeVideo;
        }
        return packet;
    }
    
    return nullptr;
}

bool FFDemuxer::write(std::shared_ptr<Packet> packet) {
    return false;
}

bool FFDemuxer::seek(uint64_t pos) {
    Mutex m(&_mutex);
    if (_formatContext == nullptr) {
        return nullptr;
    }
    
    return false;
}


