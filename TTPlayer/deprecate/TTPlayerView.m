//
//  TTPlayerView.m
//  TTPlayer
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTPlayerView.h"
#import "TTPlayerControl.h"
#import "TTVideoLayer.h"
#import "TTOpenGLLayer.h"

@interface TTPlayerView ()<TTPlayerControlDelegate>

@property (nonatomic, weak) id<TTPlayerControlDelegate> delegate;
@property (nonatomic, strong) TTPlayerControl *playerControl;
@property (nonatomic, strong) TTVideoLayer *videoLayer;
@property (nonatomic, strong) TTOpenGLLayer *openglLayer;

@property (nonatomic, strong) UIImageView *imageView;


@end

@implementation TTPlayerView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.playerControl = [[TTPlayerControl alloc] init];
        self.playerControl.delegate = self;
        
//        self.videoLayer = [[TTVideoLayer alloc] init];
//        self.videoLayer.frame = self.bounds;
//        [self.layer addSublayer:self.videoLayer];
        
        self.openglLayer = [[TTOpenGLLayer alloc] init];
        self.openglLayer.frame = self.bounds;
        [self.openglLayer setup];
        [self.layer addSublayer:self.openglLayer];
        
        self.imageView.frame = self.bounds;
        [self addSubview:self.imageView];
    }
    return self;
}

#pragma mark -- TTPlayerControlDelegate

- (void)playerControl:(TTPlayerControl *)control pixelBuffer:(void *)pixelBuffer
{
    self.videoLayer.pixelBuffer = pixelBuffer;
}

- (void)playerControl:(TTPlayerControl *)control pixels:(const UInt8 **)pixels width:(const NSUInteger)width height:(const NSUInteger)height
{
    [self.openglLayer displayPixels:pixels width:width height:height];
}

- (void)playerFinished:(TTPlayerControl *)control
{
    
}

- (void)playerControl:(TTPlayerControl *)control image:(UIImage *)image
{
    self.imageView.image = image;
}

#pragma mark getter/setter

- (UIImageView *)imageView
{
    if (_imageView == nil) {
        _imageView = [UIImageView new];
    }
    return _imageView;
}

#pragma mark public method

- (void)start {
    [self.playerControl playWithURL:self.URL];
}

@end
