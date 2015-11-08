//
//  TTAVPlayerViewEx.m
//  TTPlayer
//
//  Created by liang on 9/6/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTAVPlayerViewEx.h"
#import "TTAVPlayerCodec.h"

@interface TTAVPlayerViewEx () <TTAVPlayerCodecDelegate>

@property (nonatomic, strong) TTAVPlayerCodec *codec;
@property (nonatomic, strong) UIImageView *imageView;

@end

@implementation TTAVPlayerViewEx

- (instancetype)init {
    self = [super init];
    if (self) {
        _imageView = [[UIImageView alloc] init];
        _imageView.frame = CGRectMake(0, 0, 320, 100);
        [self addSubview:_imageView];
    }
    return self;
}

- (void)loadAssetFromFile:(NSURL *)fileURL {
    [self.codec loadAssetFromFile:fileURL];
    [self.codec start];
}

#pragma mark getter/setter
- (TTAVPlayerCodec *)codec {
    if (_codec == nil) {
        _codec = [[TTAVPlayerCodec alloc] init];
        _codec.delegate = self;
    }
    return _codec;
}

#pragma mark TTAVPlayerCodecDelegate
- (void)codec:(TTAVPlayerCodec *)codec sampleBuffer:(CMSampleBufferRef)sampleBuffer {
    CVImageBufferRef buffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    if (buffer) {
        CVPixelBufferLockBaseAddress(buffer, 0);
        
        //从 CVImageBufferRef 取得影像的细部信息
        uint8_t *base;
        size_t width, height, bytesPerRow;
        base = CVPixelBufferGetBaseAddress(buffer);
        width = CVPixelBufferGetWidth(buffer);
        height = CVPixelBufferGetHeight(buffer);
        bytesPerRow = CVPixelBufferGetBytesPerRow(buffer);
        
        if (base) {
            //利用取得影像细部信息格式化 CGContextRef
            CGColorSpaceRef colorSpace;
            CGContextRef cgContext;
            colorSpace = CGColorSpaceCreateDeviceRGB();
            cgContext = CGBitmapContextCreate(base, width, height, 8, bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
            CGColorSpaceRelease(colorSpace);
            
            //透过 CGImageRef 将 CGContextRef 转换成 UIImage
            CGImageRef cgImage;
            cgImage = CGBitmapContextCreateImage(cgContext);
            
            __weak typeof(&*self) weakSelf = self;
            dispatch_async(dispatch_get_main_queue(), ^{
                weakSelf.layer.contents = (__bridge id)(cgImage);
                CGImageRelease(cgImage);
            });
            
            CGContextRelease(cgContext);
        } else {
            NSLog(@"nil");
        }
        
        CVPixelBufferUnlockBaseAddress(buffer, 0);
    }
}

- (void)finishedCodec:(TTAVPlayerCodec *)codec {
    NSLog(@"finishedCode");
}

@end
