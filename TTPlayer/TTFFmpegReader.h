//
//  TTFFmpegReader.h
//  TTPlayerExample
//
//  Created by liang on 12/19/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "avformat.h"

#import "TTAssetReader.h"

@interface TTFFmpegReader : TTAssetReader

- (AVFrame *)nextFrame;

@end
