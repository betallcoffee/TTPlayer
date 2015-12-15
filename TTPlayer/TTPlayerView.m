//
//  TTPlayerView.m
//  TTPlayer
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTPlayerView.h"
#import "TTVideoLayer.h"
#import "TTPlayerControl.h"

@interface TTPlayerView ()<TTPlayerControlDelegate>

@property (nonatomic, weak) id<TTPlayerControlDelegate> delegate;
@property (nonatomic, strong) TTVideoLayer *videoLayer;
@property (nonatomic, strong) TTPlayerControl *playerControl;

@end

@implementation TTPlayerView

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.playerControl = [[TTPlayerControl alloc] init];
        self.playerControl.delegate = self;
        
        self.videoLayer = [[TTVideoLayer alloc] init];
        [self.layer addSublayer:self.videoLayer];
    }
    return self;
}

- (void)layoutSublayersOfLayer:(CALayer *)layer {
    self.videoLayer.frame = self.bounds;
}

#pragma mark -- TTPlayerControlDelegate

- (void)playerControl:(TTPlayerControl *)control pixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    self.videoLayer.pixelBuffer = pixelBuffer;
}

- (void)playerFinished:(TTPlayerControl *)control
{
    
}

#pragma mark getter/setter

#pragma mark public method

- (void)start {
    [self.playerControl playWithURL:self.URL];
}

@end
