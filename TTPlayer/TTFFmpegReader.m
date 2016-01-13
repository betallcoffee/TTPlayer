//
//  TTFFmpegReader.m
//  TTPlayerExample
//
//  Created by liang on 12/19/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "swscale.h"

#import "TTFFmpegReader.h"

@interface TTFFmpegReader ()
{
    AVFormatContext *_formatContext;
    AVCodecContext *_codecContext;
    AVCodec *_codec;
    
    AVPicture           _picture;
    BOOL                _pictureValid;
    struct SwsContext   *_swsContext;
}

@end

@implementation TTFFmpegReader

- (instancetype)initWithURL:(NSURL *)URL
{
    self = [super init];
    if (self) {
        av_register_all();
        const char *filename = URL.absoluteString.UTF8String;
        _formatContext = avformat_alloc_context();
        AVDictionary *formatOpts = nil;
        // 1. 读取视频的格式相关信息
        int err = avformat_open_input(&_formatContext, filename, NULL, &formatOpts);
        if (err < 0) {
            av_log(_formatContext, AV_LOG_ERROR, "error: %s/%d\n", filename, err);
        }
        
        av_format_inject_global_side_data(_formatContext);
        
        // 2. 读取其中的流信息
        err = avformat_find_stream_info(_formatContext, NULL);
        if (err < 0) {
            av_log(_formatContext, AV_LOG_ERROR, "error: %d\n", err);
        }
        
        if (true)
            av_dump_format(_formatContext, 0, filename, 0);
        
        // 3. 找出视频流的解码上下文
        _codecContext = NULL;
        for (int i = 0; i < _formatContext->nb_streams; i++) {
            AVCodecContext *codec = _formatContext->streams[i]->codec;
            if (codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                _codecContext = codec;
            }
        }
        if (_codecContext == NULL) {
            av_log(_formatContext, AV_LOG_ERROR, "No video Stream\n");
        }

        // 4. 找出解码器
        _codec = avcodec_find_decoder(_codecContext->codec_id);
        if (_codec == NULL) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not find codec\n");
        }
        
        // 5. 打开解码器
        err = avcodec_open2(_codecContext, _codec, NULL);
        if (err < 0) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not open codec\n");
        }
    }
    return self;
}

#pragma mark getter/setter
- (float)nominalFrameRate
{
    return 24;
}

#pragma mark private 
- (void) closeScaler
{
    if (_swsContext) {
        sws_freeContext(_swsContext);
        _swsContext = NULL;
    }
    
    if (_pictureValid) {
        avpicture_free(&_picture);
        _pictureValid = NO;
    }
}

- (BOOL) setupScaler
{
    [self closeScaler];
    
    _pictureValid = avpicture_alloc(&_picture,
                                    PIX_FMT_RGB24,
                                    _codecContext->width,
                                    _codecContext->height) == 0;
    
    if (!_pictureValid)
        return NO;
    
    _swsContext = sws_getCachedContext(_swsContext,
                                       _codecContext->width,
                                       _codecContext->height,
                                       _codecContext->pix_fmt,
                                       _codecContext->width,
                                       _codecContext->height,
                                       PIX_FMT_RGB24,
                                       SWS_FAST_BILINEAR,
                                       NULL, NULL, NULL);
    
    return _swsContext != NULL;
}

#pragma mark public
- (AVFrame *)nextFrame
{
    int err = 0;
    // 6. 读取一帧数据
    AVFrame *frame = av_frame_alloc();
    int gotFrame;
    do {
        AVPacket packet;
        err = av_read_frame(_formatContext, &packet);
        if (err < 0) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not read frame\n");
            return NULL;
        }
        
        err = avcodec_decode_video2(_codecContext, frame, &gotFrame, &packet);
        if (err < 0) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not decode frame\n");
            return NULL;
        }
    } while(!gotFrame);
    
    return frame;
    
//    CVPixelBufferRef pixelBuffer;
//    CFDictionaryRef attrs = (__bridge CFDictionaryRef)@{
//                                                        (id)kCVPixelBufferWidthKey: @(frame->width),
//                                                        (id)kCVPixelBufferHeightKey: @(frame->height),
//                                                        (id)kCVPixelBufferPixelFormatTypeKey: @(PIX_FMT_ARGB),
//                                                        (id)kCVPixelBufferIOSurfacePropertiesKey: @{},
//                                                        };
    
//    size_t planeWidth[] = {frame->width, frame->width/2, frame->width/2};
//    size_t planeHeight[] = {frame->height, frame->height/2, frame->height/2};
//    size_t planeBytesPerRow[] = {frame->linesize[0], frame->linesize[1], frame->linesize[2]};
//    err = CVPixelBufferCreateWithPlanarBytes(
//                                             kCFAllocatorDefault,
//                                             frame->width,
//                                             frame->height,
//                                             kCVPixelFormatType_420YpCbCr8Planar,
//                                             NULL,
//                                             0,
//                                             3,
//                                             frame->data,
//                                             planeWidth,
//                                             planeHeight,
//                                             planeBytesPerRow,
//                                             NULL,
//                                             NULL,
//                                             NULL,
//                                             &pixelBuffer);
    
//    uint8_t **buffer = malloc(2*sizeof(int *));
//    buffer[0] = frame->data[0];
//    buffer[1] = malloc(frame->linesize[0]*sizeof(int));
//    for(int i = 0; i<frame->linesize[0]; i++){
//        if(i%2){
//            buffer[1][i]=frame->data[1][i/2];
//        }else{
//            buffer[1][i]=frame->data[2][i/2];
//        }
//    }
//    CVPixelBufferCreateWithBytes(kCFAllocatorDefault,
//                                 frame->width, frame->height,
//                                 kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
//                                 buffer, frame->linesize[0], NULL, NULL, NULL, &pixelBuffer);
    
    
    // 7. 像素格式转换
//    AVFrame *frameRGB = av_frame_alloc();
//    int bufferSize = avpicture_get_size(PIX_FMT_ARGB, _codecContext->width, _codecContext->height);
//    uint8_t *buffer = malloc(bufferSize);
//    avpicture_fill((AVPicture *)frameRGB, buffer, PIX_FMT_ARGB, _codecContext->width, _codecContext->height);
//    
//    struct SwsContext *convertContext = sws_getContext(_codecContext->width,
//                                                       _codecContext->height,
//                                                       _codecContext->pix_fmt,
//                                                       _codecContext->width,
//                                                       _codecContext->height,
//                                                       PIX_FMT_ARGB,
//                                                       SWS_BICUBIC, NULL, NULL, NULL);
//    
//    sws_scale(convertContext,
//              (const uint8_t* const*)frame->data,
//              frame->linesize,
//              0,
//              _codecContext->height,
//              frameRGB->data,
//              frameRGB->linesize);
//    
//    err = CVPixelBufferCreateWithBytes(kCFAllocatorDefault,
//                                       frame->width,
//                                       frame->height,
//                                       kCVPixelFormatType_32ARGB,
//                                       frameRGB->data[0],
//                                       frameRGB->linesize[0],
//                                       NULL, NULL, attrs, &pixelBuffer);
    
//    av_frame_unref(frame);
//    return pixelBuffer;
}

@end
