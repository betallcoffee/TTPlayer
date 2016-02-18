//
//  TTPlayerFrame.m
//  TTPlayerExample
//
//  Created by liang on 2/16/16.
//  Copyright © 2016 tina. All rights reserved.
//

#import "TTPlayerFrame.h"

@implementation TTPlayerFrame

- (instancetype)initWithAVFrame:(AVFrame *)avframe andType:(TTPlayerFrameType)type
{
    self = [super init];
    if (self) {
        self.type = type;
        self.avframe = avframe;
    }
    return self;
}

- (instancetype)initWithCVBuffer:(CVPixelBufferRef)buffer andType:(TTPlayerFrameType)type
{
    self = [super init];
    if (self) {
        self.type = type;
        self.buffer = buffer;
    }
    return self;
}

- (void)dealloc
{
    if (self.avframe) {
        av_frame_unref(self.avframe);
    }
}

@end
