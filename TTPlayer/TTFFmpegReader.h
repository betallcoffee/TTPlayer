//
//  TTFFmpegReader.h
//  TTPlayerExample
//
//  Created by liang on 12/19/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "avformat.h"

#import "TTAssetReader.h"

@interface TTFFmpegReader : TTAssetReader

- (UIImage *)convertFrameToImage:(AVFrame *)avframe;

- (AVFrame *)nextFrame;

@end
