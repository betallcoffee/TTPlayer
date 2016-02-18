//
//  TTFFmpegReader.m
//  TTPlayerExample
//
//  Created by liang on 12/19/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "swscale.h"

#import "TTFFmpegReader.h"

@interface TTAVPacket : NSObject

@property (nonatomic, assign) AVPacket packet;

- (instancetype)initWithAVPacket:(AVPacket *)packet;

- (AVPacket *)packetPoint;

@end

@implementation TTAVPacket

- (instancetype)initWithAVPacket:(AVPacket *)packet
{
    self = [super init];
    if (self) {
        self.packet = *packet;
    }
    return self;
}

- (void)dealloc
{
    av_free_packet(&_packet);
}

- (AVPacket *)packetPoint
{
    return &_packet;
}

@end

@interface TTFFmpegReader ()
{
    AVFormatContext *_formatContext;
    AVCodecContext *_codecContext;
    AVCodec *_codec;
    int _videoStream;
    int _audioStream;
    
    AVPicture           _picture;
    BOOL                _pictureValid;
    struct SwsContext   *_swsContext;
}

@property (nonatomic, strong) TTQueue<TTPlayerFrame *> *videoQueue;
@property (nonatomic, strong) TTQueue<TTPlayerFrame *> *audioQueue;

@property (nonatomic, strong) TTQueue<TTAVPacket *> *videoDecodeQueue;
@property (nonatomic, strong) TTQueue<TTAVPacket *> *audioDecodeQueue;
@property (nonatomic, strong) NSThread *readThread;
@property (nonatomic, strong) NSThread *videoDecodeThread;
@property (nonatomic, strong) NSThread *audioDecodeThread;

@end

@implementation TTFFmpegReader

- (instancetype)initWithURL:(NSURL *)URL
                  andVideoQueue:(TTQueue<TTPlayerFrame *> *)videoQueue
                  andAudioQueue:(TTQueue<TTPlayerFrame *> *)audioQueue
{
    self = [super init];
    if (self) {
        self.videoQueue = videoQueue;
        self.audioQueue = audioQueue;
        
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
                _videoStream = i;
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
        
        [self setupScaler];
        
        [self setupThread];
    }
    return self;
}

#pragma mark getter/setter
- (float)nominalFrameRate
{
    return 24;
}

#pragma mark private 

- (void)setupThread
{
    self.videoDecodeQueue = [TTQueue<TTAVPacket *> new];
    self.readThread = [[NSThread alloc] initWithTarget:self selector:@selector(readThreadRoutine) object:nil];
    self.videoDecodeThread = [[NSThread alloc] initWithTarget:self
                                                     selector:@selector(videoDecodeRoutine)
                                                       object:nil];
    [self.readThread start];
    [self.videoDecodeThread start];
}

- (void)closeScaler
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

- (BOOL)setupScaler
{
    [self closeScaler];
    
    _pictureValid = avpicture_alloc(&_picture,
                                    PIX_FMT_RGB24,
                                    _codecContext->width,
                                    _codecContext->height) == 0;
    
    if (!_pictureValid)
        return NO;
    
    _swsContext = sws_getContext(_codecContext->width,
                                 _codecContext->height,
                                 _codecContext->pix_fmt,
                                 _codecContext->width,
                                 _codecContext->height,
                                 PIX_FMT_RGB24,
                                 SWS_FAST_BILINEAR, NULL, NULL, NULL);
    
    return _swsContext != NULL;
}

- (UIImage *)currentImage {
    int width = _codecContext->width;
    int height = _codecContext->height;
    
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,
                                                 _picture.data[0],
                                                 _picture.linesize[0]*height,
                                                 kCFAllocatorNull);
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(data);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImage = CGImageCreate(width,
                                       height,
                                       8,
                                       24,
                                       _picture.linesize[0],
                                       colorSpace,
                                       bitmapInfo,
                                       provider,
                                       NULL,
                                       NO,
                                       kCGRenderingIntentDefault);
    CGColorSpaceRelease(colorSpace);
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    CGImageRelease(cgImage);
    CGDataProviderRelease(provider);
    CFRelease(data);
    
    return image;
}

#pragma mark public
- (UIImage *)convertFrameToImage:(AVFrame *)avframe {
    sws_scale (_swsContext,
               (const uint8_t* const*)avframe->data,
               avframe->linesize,
               0,
               _codecContext->height,
               _picture.data,
               _picture.linesize);
    
    return [self currentImage];
}

- (AVFrame *)nextFrame
{
    int err = 0;
    // 6. 读取一帧数据
    AVFrame *avframe = av_frame_alloc();
    int gotFrame = 0;
    do {
        AVPacket packet;
        err = av_read_frame(_formatContext, &packet);
        if (err < 0) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not read frame\n");
            av_log(_codecContext, AV_LOG_ERROR, "%s", av_err2str(err));
            return NULL;
        }
        if (packet.stream_index != _videoStream) {
            continue;
        }
        
        err = avcodec_decode_video2(_codecContext, avframe, &gotFrame, &packet);
        if (err < 0) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not decode frame\n");
            av_log(_codecContext, AV_LOG_ERROR, "%s", av_err2str(err));
            return NULL;
        }
    } while(!gotFrame);
    
    return avframe;
}

#pragma mark thread routine
- (void)readThreadRoutine
{
    int err = 0;
    // 6. 读取一帧数据
    do {
        AVPacket packet;
        err = av_read_frame(_formatContext, &packet);
        if (err < 0) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not read frame\n");
            av_log(_codecContext, AV_LOG_ERROR, "%s", av_err2str(err));
            
            [self.videoDecodeQueue close];
            [self.audioDecodeQueue close];
            
            break;
        }
        
        if (packet.stream_index == _videoStream) {
            TTAVPacket *videoPacket = [[TTAVPacket alloc] initWithAVPacket:&packet];
            [self.videoDecodeQueue push:videoPacket];
        }
        
    } while(true);
}

- (void)videoDecodeRoutine
{
    int err = 0;
    int gotFrame = 0;
    do {
        TTAVPacket *videoPacket = [self.videoDecodeQueue pop];
        if (videoPacket == nil) {
            [self.videoQueue close];
            break;
        }
        
        AVFrame *avframe = av_frame_alloc();
        err = avcodec_decode_video2(_codecContext, avframe, &gotFrame, [videoPacket packetPoint]);
        if (err < 0) {
            av_log(_codecContext, AV_LOG_ERROR, "Can not decode frame\n");
            av_log(_codecContext, AV_LOG_ERROR, "%s", av_err2str(err));
            av_frame_unref(avframe);
            break;
        }
        
        TTPlayerFrame *frame = [[TTPlayerFrame alloc] initWithAVFrame:avframe andType:kTTPlayerFrameYUV];
        [self.videoQueue push:frame];
        
    } while(true);
}

- (void)audioDecodeRoutine
{
    
}

@end


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


//    av_frame_unref(frame);
//    return pixelBuffer;
