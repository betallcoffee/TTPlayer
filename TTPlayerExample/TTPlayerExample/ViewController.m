//
//  ViewController.m
//  TTPlayerExample
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTAVPlayerView.h"
#import "TTPlayerView.h"
#import "ViewController.h"

@interface ViewController ()

@property (nonatomic, strong) UIScrollView *scrollView;
@property (nonatomic, strong) TTAVPlayerView *playerView;
@property (nonatomic, strong) TTPlayerView *player;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    NSURL *fileURL = [[NSBundle mainBundle] URLForResource:@"sophie" withExtension:@"mov"];
    
//    [self.view addSubview:self.playerView];
//    self.playerView.frame = CGRectMake(0, 64, 320, 100);
//    [self.playerView loadAssetFromFile:fileURL];
//    for (int i = 0; i < 5; i++) {
//        TTAVPlayerView *playerView = [[TTAVPlayerView alloc] init];
//        [self.view addSubview:playerView];
//        playerView.frame = CGRectMake(0, 100 * (i + 1), 320, 100);
//        [playerView loadAssetFromFile:fileURL];
//    }
    
    
    [self.view addSubview:self.player];
    self.player.URL = fileURL;
    [self.player start];
    for (int i = 1; i < 1; i++) {
        TTPlayerView *playerView = [[TTPlayerView alloc] initWithFrame:CGRectMake(0, 100 * i, 320, 100)];
        [self.view addSubview:playerView];
        playerView.URL = fileURL;
        [playerView start];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark getter/setter

- (TTAVPlayerView *)playerView {
    if (_playerView == nil) {
        _playerView = [[TTAVPlayerView alloc] init];
    }
    return _playerView;
}

- (TTPlayerView *)player {
    if (_player == nil) {
        _player = [[TTPlayerView alloc] initWithFrame:CGRectMake(0, 0, 320, 200)];
    }
    return _player;
}

@end
