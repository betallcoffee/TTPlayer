//
//  TTMovieWriter.m
//  TTPlayerExample
//
//  Created by liang on 21/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

#include "easylogging++.h"
#include "TTFramebuffer.hpp"
#import "TTMovieWriter.h"

static const GLchar *const kColorSwizzlingFragmentShader = STRINGIZE
(
 precision highp float;
 varying highp vec2 v_texcoord;
 uniform sampler2D texture;
 
 void main()
 {
     gl_FragColor = texture2D(texture, v_texcoord).bgra;
 }
 );

@interface TTMovieWriter ()
{
    NSURL *_movieURL;
    NSString *_fileType;
    CGSize _videoSize;
    
    NSMutableDictionary *_outputSettings;
    
    CMTime _startTime, _previousFrameTime, _previousAudioTime;
    CMTime _offsetTime;
    
    AVAssetWriter *_assetWriter;
    AVAssetWriterInput *_assetWriterAudioInput;
    AVAssetWriterInput *_assetWriterVideoInput;
    AVAssetWriterInputPixelBufferAdaptor *_assetWriterPixelBufferInput;
}

@end

@implementation TTMovieWriter

- (id)initWithMovieURL:(NSURL *)newMovieURL size:(CGSize)newSize {
    return [self initWithMovieURL:newMovieURL
                             size:newSize
                         fileType:AVFileTypeQuickTimeMovie
                   outputSettings:nil];
}

- (instancetype)initWithMovieURL:(NSURL *)newMovieURL
                            size:(CGSize)newSize
                        fileType:(NSString *)newFileType
                  outputSettings:(NSMutableDictionary *)outputSettings {
    self = [super init];
    if (self) {
        _movieURL = [newMovieURL copy];
        _fileType = newFileType;
        _videoSize = newSize;
        _startTime = kCMTimeInvalid;
        _previousFrameTime = kCMTimeNegativeInfinity;
        _previousAudioTime = kCMTimeNegativeInfinity;
        _outputSettings = [outputSettings copy];
    }
    return self;
}

- (void)setUp {
    [self finish];
    LOG(DEBUG) << "framebuffer size: " << [self filter]->srcFramebuffer()->width() << " " << [self filter]->srcFramebuffer()->height();
    LOG(DEBUG) << "video size: " << _videoSize.width << " " << _videoSize.height;
    
    NSError *error = nil;
    _assetWriter = [[AVAssetWriter alloc] initWithURL:_movieURL fileType:_fileType error:&error];
    if (error != nil)
    {
        const char *err = [[error description] cStringUsingEncoding:NSUTF8StringEncoding];
        LOG(ERROR) << err;
    }
    
    // Set this to make sure that a functional movie is produced, even if the recording is cut off mid-stream. Only the last second should be lost in that case.
    _assetWriter.movieFragmentInterval = CMTimeMakeWithSeconds(1.0, 1000);
    
    // use default output settings if none specified
    if (_outputSettings == nil)
    {
        NSMutableDictionary *settings = [[NSMutableDictionary alloc] init];
        [settings setObject:AVVideoCodecH264 forKey:AVVideoCodecKey];
        [settings setObject:[NSNumber numberWithInt:_videoSize.width] forKey:AVVideoWidthKey];
        [settings setObject:[NSNumber numberWithInt:_videoSize.height] forKey:AVVideoHeightKey];
        _outputSettings = settings;
    }
    
    /*
     NSDictionary *videoCleanApertureSettings = [NSDictionary dictionaryWithObjectsAndKeys:
     [NSNumber numberWithInt:videoSize.width], AVVideoCleanApertureWidthKey,
     [NSNumber numberWithInt:videoSize.height], AVVideoCleanApertureHeightKey,
     [NSNumber numberWithInt:0], AVVideoCleanApertureHorizontalOffsetKey,
     [NSNumber numberWithInt:0], AVVideoCleanApertureVerticalOffsetKey,
     nil];
     
     NSDictionary *videoAspectRatioSettings = [NSDictionary dictionaryWithObjectsAndKeys:
     [NSNumber numberWithInt:3], AVVideoPixelAspectRatioHorizontalSpacingKey,
     [NSNumber numberWithInt:3], AVVideoPixelAspectRatioVerticalSpacingKey,
     nil];
     
     NSMutableDictionary * compressionProperties = [[NSMutableDictionary alloc] init];
     [compressionProperties setObject:videoCleanApertureSettings forKey:AVVideoCleanApertureKey];
     [compressionProperties setObject:videoAspectRatioSettings forKey:AVVideoPixelAspectRatioKey];
     [compressionProperties setObject:[NSNumber numberWithInt: 2000000] forKey:AVVideoAverageBitRateKey];
     [compressionProperties setObject:[NSNumber numberWithInt: 16] forKey:AVVideoMaxKeyFrameIntervalKey];
     [compressionProperties setObject:AVVideoProfileLevelH264Main31 forKey:AVVideoProfileLevelKey];
     
     [outputSettings setObject:compressionProperties forKey:AVVideoCompressionPropertiesKey];
     */
    
    _assetWriterVideoInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo
                                                                outputSettings:_outputSettings];
    
    // TODO live
    _assetWriterVideoInput.expectsMediaDataInRealTime = NO;
    
    // You need to use BGRA for the video in order to get realtime encoding. I use a color-swizzling shader to line up glReadPixels' normal RGBA output with the movie input's BGRA.
    NSDictionary *sourcePixelBufferAttributesDictionary = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
                                                           [NSNumber numberWithInt:_videoSize.width], kCVPixelBufferWidthKey,
                                                           [NSNumber numberWithInt:_videoSize.height], kCVPixelBufferHeightKey,
                                                           nil];
    //    NSDictionary *sourcePixelBufferAttributesDictionary = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt:kCVPixelFormatType_32ARGB], kCVPixelBufferPixelFormatTypeKey,
    //                                                           nil];
    
    _assetWriterPixelBufferInput = [AVAssetWriterInputPixelBufferAdaptor assetWriterInputPixelBufferAdaptorWithAssetWriterInput:_assetWriterVideoInput sourcePixelBufferAttributes:sourcePixelBufferAttributesDictionary];
    
    [_assetWriter addInput:_assetWriterVideoInput];
}

- (void)tearDown {
    
}

- (void)start {
    [self setUp];
    
    [_assetWriter startWriting];
}

- (void)finish {
    if (_assetWriter.status == AVAssetWriterStatusWriting) {
        [_assetWriterVideoInput markAsFinished];
    }

    [_assetWriter finishWritingWithCompletionHandler:^{
        
    }];
    
    [self tearDown];
}

- (void)cancel {
    if (_assetWriter.status == AVAssetWriterStatusCompleted) {
        return;
    }
    
    if (_assetWriter.status == AVAssetWriterStatusWriting) {
        [_assetWriterVideoInput markAsFinished];
    }

    [_assetWriter cancelWriting];
    
    [self tearDown];
}

- (const GLchar *)fragmentShader {
    return kColorSwizzlingFragmentShader;
}

- (void)notifyFramebufferToFilters:(int64_t)timestamp {
    CMTime frameTime = CMTimeMake(timestamp, 1000);
    if (CMTIME_IS_INVALID(_startTime)) {
        [self start];
        _startTime = frameTime;
        [_assetWriter startSessionAtSourceTime:_startTime];
    }
    
    CVPixelBufferRef pixelBuffer = NULL;
    CVReturn status = CVPixelBufferPoolCreatePixelBuffer(NULL, [_assetWriterPixelBufferInput pixelBufferPool], &pixelBuffer);
    if ((pixelBuffer == NULL) || (status != kCVReturnSuccess)) {
        LOG(WARNING) << "Create Pixel buffer failed " << status;
        CVPixelBufferRelease(pixelBuffer);
        return;
    } else {
        CVPixelBufferLockBaseAddress(pixelBuffer, 0);
        
        GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(pixelBuffer);
        glReadPixels(0, 0, _videoSize.width, _videoSize.height,
                     GL_RGBA, GL_UNSIGNED_BYTE, pixelBufferData);
    }
    
    if (!_assetWriterVideoInput.readyForMoreMediaData) {
        LOG(DEBUG) << "Assert writer did not ready for more media datd: " << timestamp;
    } else if(_assetWriter.status == AVAssetWriterStatusWriting) {
        if ([_assetWriterPixelBufferInput appendPixelBuffer:pixelBuffer withPresentationTime:frameTime]) {
            LOG(TRACE) << "Appending pixel buffer at time: " << timestamp;
        } else {
            LOG(WARNING) << "Problem appending pixel buffer at time: " << timestamp;
        }
    } else {
        LOG(WARNING) << "Couldn't write a frame at time: " << timestamp;
    }

    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    CVPixelBufferRelease(pixelBuffer);

    _previousFrameTime = frameTime;
}

@end
