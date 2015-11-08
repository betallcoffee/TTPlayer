//
//  TTAVPlayerCodec.m
//  TTPlayer
//
//  Created by liang on 9/6/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import <VideoToolbox/VideoToolbox.h>
#import "TTAVPlayerCodec.h"

@interface TTAVPlayerCodec ()
{
    NSThread *_thread;
    AVAssetReader *_assetReader;
    AVAssetTrack *_track;
    AVAssetReaderTrackOutput *_trackOutput;
}

@end

@implementation TTAVPlayerCodec

- (instancetype)init {
    self = [super init];
    if (self) {
        _thread = [[NSThread alloc] initWithTarget:self selector:@selector(codecRoutine) object:nil];
    }
    return self;
}

- (void)loadAssetFromFile:(NSURL *)fileURL {
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:fileURL options:nil];
    NSError *error;
    _assetReader = [[AVAssetReader alloc] initWithAsset:asset error:&error];
    NSArray *tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
    _track = tracks[0];
    NSDictionary *options = @{(__bridge_transfer NSString *)kCVPixelBufferPixelFormatTypeKey:@(kCVPixelFormatType_32BGRA)};
    _trackOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:_track outputSettings:options];
    [_assetReader addOutput:_trackOutput];
}

- (void)start {
    [_assetReader startReading];
    [_thread start];
}

- (void)codecRoutine {
    while (_assetReader.status == AVAssetReaderStatusReading &&
           _track.nominalFrameRate > 0) {
        if (_delegate) {
            CMSampleBufferRef buffer = [_trackOutput copyNextSampleBuffer];
            if (buffer) {
                [_delegate codec:self sampleBuffer:buffer];
                CFRelease(buffer);
            }
        }
        [NSThread sleepForTimeInterval:1/_track.nominalFrameRate];
    }
    if (_delegate) {
        [_delegate finishedCodec:self];
    }
}

@end
