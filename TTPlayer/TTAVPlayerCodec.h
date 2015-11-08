//
//  TTAVPlayerCodec.h
//  TTPlayer
//
//  Created by liang on 9/6/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

@class TTAVPlayerCodec;

@protocol TTAVPlayerCodecDelegate <NSObject>

@required
- (void)codec:(TTAVPlayerCodec *)codec sampleBuffer:(CMSampleBufferRef)sampleBuffer;
- (void)finishedCodec:(TTAVPlayerCodec *)codec;

@end

@interface TTAVPlayerCodec : NSObject

@property (nonatomic, weak) id<TTAVPlayerCodecDelegate> delegate;

- (void)loadAssetFromFile:(NSURL *)fileURL;

- (void)start;

@end
