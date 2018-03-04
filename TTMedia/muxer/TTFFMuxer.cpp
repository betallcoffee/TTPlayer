//
//  TTFFMuxer.cpp
//  TTPlayerExample
//
//  Created by liang on 19/2/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "easylogging++.h"

#include "TTFFMuxer.hpp"

using namespace TT;

FFMuxer::FFMuxer() {
    
}

FFMuxer::~FFMuxer() {
    close();
}

bool FFMuxer::open(std::shared_ptr<URL> url, AVCodecParameters *audioCodecParam, AVCodecParameters *videoCodecParam) {
    _url = url;
    int ret = avformat_alloc_output_context2(&_formatContext, nullptr, nullptr, _url->cStr());
    if (ret < 0) {
        LOG(ERROR) << "Muxer FFMuxer open failed:" << av_err2str(ret) << " url:" << url;
        close();
        return false;
    }
    
//    if (audioCodecParam) {
//        AVCodec *codec = avcodec_find_encoder(audioCodecParam->codec_id);
//        _audioStream = avformat_new_stream(_formatContext, codec);
//        if (_audioStream) {
//            avcodec_parameters_copy(_audioStream->codecpar, audioCodecParam);
//        }
//    }
    
    if (videoCodecParam) {
        AVCodec *codec = avcodec_find_encoder(videoCodecParam->codec_id);
        unsigned int tag = 0;
        if (av_codec_get_tag2(_formatContext->oformat->codec_tag, videoCodecParam->codec_id, &tag) == 0) {
            LOG(ERROR) << "Muxer FFMuxer av_codec_get_tag2 failed: " << videoCodecParam->codec_id;
        }
        videoCodecParam->codec_tag = tag;
        _videoStream = avformat_new_stream(_formatContext, codec);
        if (_videoStream) {
            avcodec_parameters_copy(_videoStream->codecpar, videoCodecParam);
        }
    }
    
    av_dump_format(_formatContext, 0, _url->cStr(), 1);
    
    if (!(_formatContext->flags & AVFMT_NOFILE)) {
        ret = avio_open(&_formatContext->pb, _url->cStr(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOG(ERROR) << "Could not open output file:" << _url->cStr() << " error:" << av_err2str(ret);
            close();
            return false;
        }
    }
    
    ret = avformat_write_header(_formatContext, NULL);
    if (ret < 0) {
        LOG(ERROR) << "Error when write header file:" << _url->cStr() << " error:" << av_err2str(ret);
        close();
        return false;
    }
    
    return true;
}

void FFMuxer::close() {
    LOG(INFO) << "Muxer FFMuxer close";
    if (_formatContext) {
        av_write_trailer(_formatContext);
        avformat_close_input(&_formatContext);
        avformat_free_context(_formatContext);
    }
}

bool FFMuxer::write(std::shared_ptr<Packet> packet) {
    int ret = av_interleaved_write_frame(_formatContext, packet->avpacket());
    if (ret < 0) {
        LOG(ERROR) << "Error muxing packet";
        return false;
    }
    return true;
}
