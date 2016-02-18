//
//  TTPlayerControl.m
//  TTPlayerExample
//
//  Created by liang on 12/14/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTAssetReader.h"
#import "TTFFmpegReader.h"
#import "TTPlayerFrame.h"
#import "TTQueue.h"

#import "TTPlayerControl.h"

@interface TTPlayerControl ()

@property (nonatomic, strong) TTAssetReader *reader;
@property (nonatomic, strong) TTFFmpegReader *ffReader;
@property (nonatomic, strong) TTQueue<TTPlayerFrame *> *videoQueue;
@property (nonatomic, strong) TTQueue<TTPlayerFrame *> *audioQueue;
@property (nonatomic, strong) NSThread *thread;

@end

@implementation TTPlayerControl

- (instancetype)init
{
    self = [super init];
    if (self) {
        [self setupQueue];
    }
    return self;
}

- (void)playWithURL:(NSURL *)URL
{
    if ([URL isFileURL]) {
        self.ffReader = [[TTFFmpegReader alloc] initWithURL:URL
                                              andVideoQueue:self.videoQueue
                                              andAudioQueue:self.audioQueue];
        
//        self.reader = [[TTAssetReader alloc] initWithURL:URL];
        self.thread = [[NSThread alloc] initWithTarget:self selector:@selector(ffmepgRoutine) object:nil];
        [self.thread start];
    }
}

#pragma mark private method
- (void)setupQueue
{
    self.videoQueue = [TTQueue<TTPlayerFrame *> new];
    self.audioQueue = [TTQueue<TTPlayerFrame *> new];
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
    
    do {
        TTPlayerFrame *frame = [self.videoQueue pop];
        if (frame == nil) {
            break;
        }
        
        if (self.delegate) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self.delegate playerControl:self
                                      pixels:frame.avframe->data
                                       width:frame.avframe->width
                                      height:frame.avframe->height];
            });
        }
    } while (true);
    
    if (self.delegate) {
        [self.delegate playerFinished:self];
    }
}

@end
