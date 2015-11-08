//
//  TTAVPlayerView.m
//  TTPlayer
//
//  Created by liang on 9/5/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "TTAVPlayerView.h"

static const NSString *kItemStatusContext;

@interface TTAVPlayerView ()

@property (nonatomic, strong) AVPlayer *player;
@property (nonatomic, strong) AVPlayerItem *playerItem;

@end

@implementation TTAVPlayerView

+ (Class)layerClass {
    return [AVPlayerLayer class];
}

- (void)loadAssetFromFile:(NSURL *)fileURL {
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:fileURL options:nil];
    [asset loadValuesAsynchronouslyForKeys:@[@"tracks"] completionHandler:^{
        NSError *error;
        AVKeyValueStatus status = [asset statusOfValueForKey:@"tracks" error:&error];
        if (status == AVKeyValueStatusLoaded) {
            self.playerItem = [AVPlayerItem playerItemWithAsset:asset];
            [self.playerItem addObserver:self forKeyPath:@"status"
                                 options:NSKeyValueObservingOptionInitial
                                 context:&kItemStatusContext];
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                     selector:@selector(onAVPlayerItemDidPlayToEndTimeNotification:)
                                                         name:AVPlayerItemDidPlayToEndTimeNotification
                                                       object:self.playerItem];
            self.player = [AVPlayer playerWithPlayerItem:self.playerItem];
            [self.player play];
        }
    }];
}

#pragma mark KVO

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context {
    if (context == &kItemStatusContext) {
        NSLog(@"observeValueForKeyPath:%@", keyPath);
    } else {
        [super observeValueForKeyPath:keyPath
                             ofObject:object
                               change:change
                              context:context];
    }
}

#pragma mark notification 

- (void)onAVPlayerItemDidPlayToEndTimeNotification:(NSNotification *)aNotification {
    
}


#pragma mark getter/setter

- (AVPlayer *)player {
    return ((AVPlayerLayer *)self.layer).player;
}

- (void)setPlayer:(AVPlayer *)player {
    ((AVPlayerLayer *)self.layer).player = player;
}

@end
