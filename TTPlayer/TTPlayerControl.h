//
//  TTPlayerControl.h
//  TTPlayerExample
//
//  Created by liang on 12/14/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreVideo/CoreVideo.h>

@class TTPlayerControl;

@protocol TTPlayerControlDelegate <NSObject>

- (void)playerControl:(TTPlayerControl *)control pixelBuffer:(CVPixelBufferRef)pixelBuffer;
- (void)playerFinished:(TTPlayerControl *)control;

@end

@interface TTPlayerControl : NSObject

@property (nonatomic, weak) id<TTPlayerControlDelegate> delegate;

- (void)playWithURL:(NSURL *)URL;

@end
