//
//  TTAudioCodec.cpp
//  TTPlayerExample
//
//  Created by liang on 10/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include <algorithm>

#include "TTAudioCodec.hpp"

using namespace TT;

AudioCodec::AudioCodec(AVStream *avStream) :
 _avStream(avStream),
 _avCodec(nullptr), _avFrame(nullptr), _swrContext(nullptr),
 _sampleRate(0), _channelLayout(0), _fmt(AV_SAMPLE_FMT_NONE), _nbSamples(0), _cb(nullptr) {
     if (_avStream) {
         _avCodecContext = _avStream->codec;
     }
}

AudioCodec::~AudioCodec() {
    close();
}

bool AudioCodec::open() {
    if (_avCodecContext) {
        _avCodec = avcodec_find_decoder(_avCodecContext->codec_id);
        if (_avCodec) {
            if (avcodec_open2(_avCodecContext, _avCodec, NULL) == 0) {
                _avFrame = av_frame_alloc();
                return true;
            }
        }
    }
    return false;
}

void AudioCodec::close() {
    if (_avFrame) {
        av_frame_free(&_avFrame);
        _avFrame = nullptr;
    }
    if (_avCodecContext) {
        avcodec_close(_avCodecContext);
        _avCodec = nullptr;
    }
}

void AudioCodec::createSwrContext() {
    if (_swrContext) {
        swr_close(_swrContext);
        swr_free(&_swrContext);
        _swrContext = NULL;
    }
    
    _swrContext = swr_alloc();
    swr_alloc_set_opts(_swrContext,
                       _channelLayout,
                       AV_SAMPLE_FMT_FLT,
                       _sampleRate,
                       _channelLayout,
                       _fmt,
                       _sampleRate,0, NULL);
    swr_init(_swrContext);
}

std::shared_ptr<Frame> AudioCodec::decode(std::shared_ptr<Packet> packet) {
    int gotFrame = 0;
    int ret = avcodec_decode_audio4(_avCodecContext, _avFrame, &gotFrame, packet->avpacket());
    if (ret < 0)
    {
        printf("Error decoding audio frame (%d), (%s)", ret, av_err2str(ret));
    }
    
    if (gotFrame) {
        int64_t dec_channel_layout =
        (_avFrame->channel_layout && av_frame_get_channels(_avFrame) == av_get_channel_layout_nb_channels(_avFrame->channel_layout)) ?
        _avFrame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(_avFrame));
        
        if (_swrContext             == NULL            ||
            _avFrame->format        != _fmt            ||
            dec_channel_layout      != _channelLayout ||
            _avFrame->sample_rate   != _sampleRate) {
            _fmt = (AVSampleFormat)_avFrame->format;
            _channelLayout = dec_channel_layout;
            _sampleRate = _avFrame->sample_rate;
            createSwrContext();
            if (_cb) {
                AudioDesc desc;
                desc.sampleRate = _sampleRate;
                desc.channels = _avFrame->channels;
                desc.fmt = _fmt;
                
                _nbSamples = std::max(_avCodecContext->frame_size, _avFrame->nb_samples);
                desc.nbSamples = _nbSamples;
                _cb(desc);
            }
        }
        
        int outLinesize;
        int outBuffSize = av_samples_get_buffer_size(&outLinesize,
                                                     _avFrame->channels,
                                                     _avFrame->nb_samples + 256,
                                                     AV_SAMPLE_FMT_FLT, 1);
        // TODO reuse outBuff;
        std::shared_ptr<Frame> frame = std::make_shared<Frame>();
        uint8_t *outBuff = (uint8_t *)malloc(outBuffSize);
        if (frame->reallocData(outBuffSize, 0)) {
            int nbSamples = swr_convert(_swrContext,
                                        &frame->data[0],
                                        outLinesize,
                                        (const uint8_t **)_avFrame->extended_data,
                                        _avFrame->nb_samples);
            
            int outSize = nbSamples * _avFrame->channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT);
            outSize = outSize <= outBuffSize ? outSize : outBuffSize;
            
            frame->lineSize[0] = outSize;
            frame->pts = packet->pts;
            frame->sampleFormat = _fmt;
            if (_avFrame->pts != AV_NOPTS_VALUE) {
                AVRational tb = av_codec_get_pkt_timebase(_avCodecContext);
                frame->pts = _avFrame->pts * av_q2d(tb) * 1000;
            } else {
                AVRational tb = _avStream->time_base;
                frame->pts = _avFrame->pkt_pts * av_q2d(tb) * 1000;
            }
            
            return frame;
        }
    }
    
    return nullptr;
}
