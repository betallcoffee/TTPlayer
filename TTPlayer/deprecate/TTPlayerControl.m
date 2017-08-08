//
//  TTPlayerControl.m
//  TTPlayerExample
//
//  Created by liang on 12/14/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#include "time.h"

#import "TTAssetReader.h"
#import "TTFFmpegReader.h"
#import "TTPlayerFrame.h"
#import "TTQueue.h"

#import "TTPlayerControl.h"

@interface TTPlayerControl ()

@property (nonatomic, strong) TTAssetReader *reader;
@property (nonatomic, strong) TTFFmpegReader *ffReader;
@property (nonatomic, strong) TTQueue<TTPlayerFrame *> *videoQueue;
@property (nonatomic, assign) double videoTime;
@property (nonatomic, assign) double nextFramePTS;
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

- (void)saveImage:(UIImage *)image toFile:(NSString *)filePath
{
    NSData *data = UIImageJPEGRepresentation(image, 1.0);
    [data writeToFile:filePath atomically:YES];
}

- (void)ffmepgRoutine {
    NSArray *dirs = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDir = dirs[0];
    NSLog(@"cacheDir:%@", cacheDir);
    
    self.videoTime = ((double)av_gettime()) / TIME_MICROS_MAX;
    self.nextFramePTS = -1;
    do {
        if (self.nextFramePTS < 0) {
            TTPlayerFrame *first = [self.videoQueue first];
            if (first == nil) {
                continue;
            }
            self.nextFramePTS = first.pts;
            first = nil;
        }
        
        double now = ((double)av_gettime()) / TIME_MICROS_MAX;
        if (self.videoTime + self.nextFramePTS > now ) {
            continue;
        } else {
            self.nextFramePTS = -1;
        }
        
        TTPlayerFrame *frame = [self.videoQueue pop];
        if (frame == nil) {
            break;
        }
        
//        NSLog(@"POP PTS:%lf, %p", frame.pts, frame.avframe);
//        NSString *filename = [NSString stringWithFormat:@"PTS_%lf_2", frame.pts];
//        NSString *filePath = [cacheDir stringByAppendingPathComponent:filename];
//        UIImage *image = [self.ffReader convertFrameToImage:frame.avframe];
//        [self saveImage:image toFile:filePath];
        
        if (self.delegate) {
            dispatch_sync(dispatch_get_main_queue(), ^{
//                NSLog(@"mai PTS:%lf, %p", frame.pts, frame.avframe);
                [self.delegate playerControl:self
                                      pixels:frame.avframe->data
                                       width:frame.avframe->width
                                      height:frame.avframe->height];
            });
        }
        frame = nil;
    } while (true);
    
    if (self.delegate) {
        [self.delegate playerFinished:self];
    }
}

@end
