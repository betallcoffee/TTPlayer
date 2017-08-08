//
//  TTVideoLayer.m
//  TTPlayerExample
//
//  Created by liang on 12/14/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTVideoLayer.h"

@implementation TTVideoLayer

- (void)setPixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    if (pixelBuffer) {
        CVPixelBufferLockBaseAddress(pixelBuffer, 0);
        
        //从 CVImageBufferRef 取得影像的细部信息
        uint8_t *base;
        size_t width, height, bytesPerRow;
        base = CVPixelBufferGetBaseAddress(pixelBuffer);
        width = CVPixelBufferGetWidth(pixelBuffer);
        height = CVPixelBufferGetHeight(pixelBuffer);
        bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);
        
        if (base) {
            //利用取得影像细部信息格式化 CGContextRef
            CGColorSpaceRef colorSpace;
            CGContextRef cgContext;
            colorSpace = CGColorSpaceCreateDeviceRGB();
            cgContext = CGBitmapContextCreate(base, width, height, 8, bytesPerRow, colorSpace,
                                              kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
            CGColorSpaceRelease(colorSpace);
            
            //透过 CGImageRef 将 CGContextRef 转换成 UIImage
            CGImageRef cgImage;
            cgImage = CGBitmapContextCreateImage(cgContext);
            
            if (cgImage) {
                __weak typeof(&*self) weakSelf = self;
                dispatch_async(dispatch_get_main_queue(), ^{
                    weakSelf.contents = (__bridge id)(cgImage);
                    CGImageRelease(cgImage);
                });
            } else {
                NSLog(@"create cgImage nil");
            }
            
            CGContextRelease(cgContext);
        } else {
            NSLog(@"nil");
        }
        
        CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    }
}

@end
