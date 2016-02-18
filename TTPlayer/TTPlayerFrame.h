//
//  TTPlayerFrame.h
//  TTPlayerExample
//
//  Created by liang on 2/16/16.
//  Copyright © 2016 tina. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <CoreVideo/CoreVideo.h>
#import "avformat.h"

typedef enum : NSUInteger {
    kTTPlayerFrameRGB,
    kTTPlayerFrameYUV,
} TTPlayerFrameType;

@interface TTPlayerFrame : NSObject

@property (nonatomic, assign) TTPlayerFrameType type;
@property (nonatomic, assign) CVPixelBufferRef buffer;
@property (nonatomic, assign) AVFrame *avframe;

- (instancetype)initWithAVFrame:(AVFrame *)avframe andType:(TTPlayerFrameType)type;

- (instancetype)initWithCVBuffer:(CVPixelBufferRef)buffer andType:(TTPlayerFrameType)type;

@end
