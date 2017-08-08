//
//  TTPlayerControl.h
//  TTPlayerExample
//
//  Created by liang on 12/14/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <CoreVideo/CoreVideo.h>

@class TTPlayerControl;

@protocol TTPlayerControlDelegate <NSObject>

- (void)playerControl:(TTPlayerControl *)control pixelBuffer:(void *)pixelBuffer;
- (void)playerControl:(TTPlayerControl *)control pixels:(const UInt8 **)pixels width:(const NSUInteger)width height:(const NSUInteger)height;
- (void)playerFinished:(TTPlayerControl *)control;

- (void)playerControl:(TTPlayerControl *)control image:(UIImage *)image;

@end

@interface TTPlayerControl : NSObject

@property (nonatomic, weak) id<TTPlayerControlDelegate> delegate;

- (void)playWithURL:(NSURL *)URL;

@end
