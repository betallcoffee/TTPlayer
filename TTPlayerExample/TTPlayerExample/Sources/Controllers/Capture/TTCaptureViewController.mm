//
//  TTCaptureViewController.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTPlayer_ios.h"
#include "TTProcess.h"
#include "TTCapture.h"

#import "TTCaptureViewController.h"
#import "TTFileManager.h"

@interface TTCaptureViewController ()
{
    std::shared_ptr<FilterGroup> _filterGroup;
    
    TTImageView *_imageView;
    TTMovieWriter *_movieWriter;
}

@property (nonatomic, strong) TTCapture *capture;
@property (nonatomic, strong) UIButton *captureButton;

@end

@implementation TTCaptureViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [self setupFilter];
    [self setupCapture];
}

- (void)viewWillDisappear:(BOOL)animated {
    [TTFileManager sharedInstance].movieURLList = nil;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

#pragma mark -
#pragma mark UI

- (void)setupUI {
    self.view.backgroundColor = [UIColor blackColor];
    
    _imageView = [TTImageView new];
    _imageView.frame = self.view.bounds;
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:_imageView];
    
    self.captureButton.bounds = CGRectMake(0, 0, 44, 44);
    self.captureButton.backgroundColor = [UIColor redColor];
    self.captureButton.center = CGPointMake(self.view.center.x, self.view.center.y/2*3);
    [self.captureButton addTarget:self action:@selector(onClickCapture:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.captureButton];
}

- (void)setupFilter {
    _filterGroup = std::make_shared<FilterGroup>();
    _filterGroup->addFilter([_imageView filter]);
    
    NSURL *movieURL = [[TTFileManager sharedInstance] newMovieURL];
    _movieWriter = [[TTMovieWriter alloc] initWithMovieURL:movieURL size:CGSizeMake(640, 480)];
    _filterGroup->addFilter([_movieWriter filter], 1);
}

- (void)setupCapture {
    self.capture.outputImageOrientation = AVCaptureVideoOrientationPortrait;
    [self.capture addFilter:_filterGroup];
}

#pragma mark -
#pragma mark ViewModel

- (void)configViewModel {

}

#pragma mark -
#pragma mark setter/getter

- (TTCapture *)capture {
    if (_capture == nil) {
        _capture = [TTCapture new];
    }
    return _capture;
}

- (UIButton *)captureButton {
    if (_captureButton == nil) {
        _captureButton = [UIButton new];
    }
    return _captureButton;
}

#pragma mark -
#pragma mark target/action

- (void)onClickCapture:(UIButton *)button {
    if (button.selected) {
        button.selected = NO;
        [self.capture stopCameraCapture];
    } else {
        button.selected = YES;
        [self.capture startCameraCapture];
    }
}

@end
