//
//  TTVideoLayer.h
//  TTPlayerExample
//
//  Created by liang on 12/14/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#include <CoreVideo/CoreVideo.h>

@interface TTVideoLayer : CALayer

@property(nonatomic) CVPixelBufferRef pixelBuffer;

@end
