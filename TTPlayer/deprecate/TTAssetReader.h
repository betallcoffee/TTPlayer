//
//  TTAssetReader.h
//  TTPlayerExample
//
//  Created by liang on 12/15/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <CoreVideo/CoreVideo.h>
#import <Foundation/Foundation.h>

#import "TTPlayerFrame.h"
#import "TTQueue.h"

@interface TTAssetReader : NSObject

/* indicates the frame rate of the track, in units of frames per second */
@property (nonatomic, readonly) float nominalFrameRate;

- (instancetype)initWithURL:(NSURL *)URL;

- (instancetype)initWithURL:(NSURL *)URL
                  andVideoQueue:(TTQueue<TTPlayerFrame *>*)videoQueue
                  andAudioQueue:(TTQueue<TTPlayerFrame *>*)audioQueue;

- (void *)readNextBuffer;

@end
