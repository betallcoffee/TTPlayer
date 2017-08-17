//
//  ViewController.m
//  TTPlayerExample
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#include <memory>

#import "TTPlayer_ios.h"

#import "TTAVPlayerView.h"
#import "TTPlayerView.h"
#import "ViewController.h"

#include "TTProcess.h"

using namespace TT;

@interface ViewController ()
{
    Player *_player;
    
    std::shared_ptr<ContrastFilter> _contrast;
    std::shared_ptr<ImageView> _imageView;
}

@property (nonatomic, strong) UIScrollView *scrollView;
@property (nonatomic, strong) TTOpenGLView *glView;
@property (nonatomic, strong) TTAVPlayerView *avplayerView;
@property (nonatomic, strong) TTPlayerView *playerView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
//    self.glView.frame = self.view.bounds;
//    [self.view addSubview:self.glView];
    
    _contrast = std::make_shared<ContrastFilter>();
    
    _imageView = std::make_shared<ImageView>();
    _imageView->imageView().frame = self.view.bounds;
    _imageView->imageView().contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:_imageView->imageView()];
    _contrast->addFilter(_imageView);
    
    _player = createPlayer_ios();
    bindGLView_ios(_player, self.glView);
    _player->bindFilter(_contrast);
    
//    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"mp4"];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"audio_HEv2" ofType:@"flv"];
    const char *cFilePath = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
    std::shared_ptr<URL> url = std::make_shared<URL>(cFilePath);
//    std::shared_ptr<URL> url = std::make_shared<URL>("rtmp://live.hkstv.hk.lxdns.com/live/hks");
    _player->play(url);
    
//    NSURL *fileURL = [[NSBundle mainBundle] URLForResource:@"video" withExtension:@"h264"];

//    [self.view addSubview:self.playerView];
//    self.avplayerView.frame = CGRectMake(0, 64, 320, 100);
//    [self.avplayerView loadAssetFromFile:fileURL];
//    for (int i = 0; i < 5; i++) {
//        TTAVPlayerView *playerView = [[TTAVPlayerView alloc] init];
//        [self.view addSubview:playerView];
//        playerView.frame = CGRectMake(0, 100 * (i + 1), 320, 100);
//        [playerView loadAssetFromFile:fileURL];
//    }
    
    
//    [self.view addSubview:self.playerView];
//    self.player.URL = fileURL;
//    [self.player start];
//    for (int i = 1; i < 1; i++) {
//        TTPlayerView *playerView = [[TTPlayerView alloc] initWithFrame:CGRectMake(0, 100 * i, 320, 100)];
//        [self.view addSubview:playerView];
//        playerView.URL = fileURL;
//        [playerView start];
//    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark getter/setter

- (TTOpenGLView *)glView {
    if (_glView == nil) {
        _glView = [TTOpenGLView new];
    }
    return _glView;
}

- (TTAVPlayerView *)playerView {
    if (_avplayerView == nil) {
        _avplayerView = [[TTAVPlayerView alloc] init];
    }
    return _avplayerView;
}

- (TTPlayerView *)player {
    if (_playerView == nil) {
        _playerView = [[TTPlayerView alloc] initWithFrame:CGRectMake(0, 0, 320, 200)];
    }
    return _playerView;
}

@end
