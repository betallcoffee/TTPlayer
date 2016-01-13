//
//  TTPlayerControl.m
//  TTPlayerExample
//
//  Created by liang on 12/14/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTAssetReader.h"
#import "TTFFmpegReader.h"
#import "TTPlayerControl.h"

@interface TTPlayerControl ()

@property (nonatomic, strong) TTAssetReader *reader;
@property (nonatomic, strong) TTFFmpegReader *ffReader;
@property (nonatomic, strong) NSThread *thread;

@end

@implementation TTPlayerControl

- (void)playWithURL:(NSURL *)URL
{
    if ([URL isFileURL]) {
        self.ffReader = [[TTFFmpegReader alloc] initWithURL:URL];
//        self.reader = [[TTAssetReader alloc] initWithURL:URL];
        self.thread = [[NSThread alloc] initWithTarget:self selector:@selector(ffmepgRoutine) object:nil];
        [self.thread start];
    }
}

- (void)playRoutine {
    CVImageBufferRef buffer;
    while ((buffer = [self.reader readNextBuffer])) {
        if (self.delegate) {
            if (buffer) {
                [self.delegate playerControl:self pixelBuffer:buffer];
                CFRelease(buffer);
            }
        }
        [NSThread sleepForTimeInterval:1/self.reader.nominalFrameRate];
    }
    if (self.delegate) {
        [self.delegate playerFinished:self];
    }

}

- (void)ffmepgRoutine {
    AVFrame *frame;
    while ((frame = [self.ffReader nextFrame])) {
        if (self.delegate) {
            if (frame) {
                [self.delegate playerControl:self pixels:frame->data width:frame->width height:frame->height];
                av_frame_unref(frame);
            }
        }
//        [NSThread sleepForTimeInterval:1/self.reader.nominalFrameRate];
    }
    if (self.delegate) {
        [self.delegate playerFinished:self];
    }
}

@end
