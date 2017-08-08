//
//  TTOpenGLLayer.h
//  TTPlayerExample
//
//  Created by liang on 12/21/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AAPLEAGLLayer.h"

@interface TTOpenGLLayer : CAEAGLLayer

- (void)setup;

- (void)displayPixels:(const UInt8 **)pixels width:(const NSUInteger)width height:(const NSUInteger)height;

@end
