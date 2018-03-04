//
//  TTCapture.h
//  TTPlayerExample
//
//  Created by liang on 1/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#undef AVMediaType
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#include "TTProcess.h"

#import "TTVideoLayer.h"

@interface TTCapture : NSObject

// Test
@property (nonatomic, strong) TTVideoLayer *videoLayer;

@property (nonatomic, strong) TTMovieWriter *movieWriter;

// This enables the capture session preset to be changed on the fly
@property (nonatomic, copy) NSString *captureSessionPreset;
/// This determines the rotation applied to the output image, based on the source material
@property (nonatomic, assign) AVCaptureVideoOrientation outputImageOrientation;

- (void)addFilter:(std::shared_ptr<TT::Filter>)filter;

- (void)startCaptureVideo:(BOOL)hasVideo andAudio:(BOOL)hasAudio;
- (void)stopCameraCapture;

@end
