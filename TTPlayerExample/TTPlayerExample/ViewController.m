//
//  ViewController.m
//  TTPlayerExample
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import "TTAVPlayerView.h"
#import "TTAVPlayerViewEx.h"
#import "ViewController.h"

@interface ViewController ()

@property (nonatomic, strong) TTAVPlayerView *playerView;
@property (nonatomic, strong) TTAVPlayerViewEx *playerViewEx;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    NSURL *fileURL = [[NSBundle mainBundle] URLForResource:@"test" withExtension:@"MOV"];
    
    [self.view addSubview:self.playerView];
    self.playerView.frame = CGRectMake(0, 64, 320, 100);
    [self.playerView loadAssetFromFile:fileURL];
    
//    for (int i = 0; i < 5; i++) {
//        TTAVPlayerView *playerView = [[TTAVPlayerView alloc] init];
//        [self.view addSubview:playerView];
//        playerView.frame = CGRectMake(0, 100 * (i + 1), 320, 100);
//        [playerView loadAssetFromFile:fileURL];
//    }
    
//    [self.view addSubview:self.playerViewEx];
//    self.playerViewEx.frame = CGRectMake(0, 200, 320, 100);
//    [self.playerViewEx loadAssetFromFile:fileURL];
//    for (int i = 0; i < 7; i++) {
//        TTAVPlayerViewEx *playerView = [[TTAVPlayerViewEx alloc] init];
//        [self.view addSubview:playerView];
//        playerView.frame = CGRectMake(0, 100 * (i + 1), 320, 100);
//        [playerView loadAssetFromFile:fileURL];
//    }
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

- (TTAVPlayerViewEx *)playerViewEx {
    if (_playerViewEx == nil) {
        _playerViewEx = [[TTAVPlayerViewEx alloc] init];
    }
    return _playerViewEx;
}

@end
