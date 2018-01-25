//
//  TTCaptureViewController.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTPlayer_ios.h"
#include "TTProcess.h"
#import "TTCapture.h"
#import "TTCaptureButton.h"

#import "TTCaptureViewController.h"
#import "TTFileManager.h"

@interface TTCaptureViewController ()
{
    std::shared_ptr<TT::FilterGroup> _filterGroup;
    std::shared_ptr<TT::BrightnessFilter> _brightnessFilter;
    std::shared_ptr<TT::ContrastFilter> _contrastFilter;
    std::shared_ptr<TT::SaturationFilter> _saturationFilter;
    
    TTImageView *_imageView;
    TTMovieWriter *_movieWriter;
}

@property (nonatomic, strong) TTCapture *capture;
@property (nonatomic, strong) TTCaptureButton *captureButton;

@property (nonatomic, strong) UILabel *brightnessLabel;
@property (nonatomic, strong) UISlider *brightnessSlider;
@property (nonatomic, strong) UILabel *contrastLabel;
@property (nonatomic, strong) UISlider *contrastSlider;
@property (nonatomic, strong) UILabel *saturationLabel;
@property (nonatomic, strong) UISlider *saturationSlider;

@end

@implementation TTCaptureViewController

- (void)dealloc {
    _filterGroup.reset();
    _brightnessFilter.reset();
    _contrastFilter.reset();
    _saturationFilter.reset();
}

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
    self.view.backgroundColor = [UIColor whiteColor];
    
    _imageView = [TTImageView new];
    _imageView.contentMode = UIViewContentModeScaleAspectFit;
    [self.view addSubview:_imageView];
    [_imageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(_imageView.superview);
    }];
    
    [self.captureButton addTarget:self
                           action:@selector(onClickCapture:)
                 forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.captureButton];
    [self.captureButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.mas_equalTo(60);
        make.height.mas_equalTo(60);
        make.centerX.equalTo(self.captureButton.superview);
        make.bottom.equalTo(self.captureButton.superview).with.offset(-50);
    }];
    
    self.brightnessLabel.text = @"bright";
    self.brightnessLabel.textColor = [UIColor whiteColor];
    [self.view addSubview:self.brightnessLabel];
    [self.brightnessLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.equalTo(self.brightnessLabel.superview).with.offset(5);
        make.bottom.equalTo(self.captureButton.mas_top).with.offset(-40);
    }];
    
    self.brightnessSlider.minimumValue = -1.0;
    self.brightnessSlider.maximumValue = 1.0;
    self.brightnessSlider.value = 0.0;
    [self.brightnessSlider addTarget:self
                              action:@selector(onBrightnessSlider:)
                    forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:self.brightnessSlider];
    [self.brightnessSlider mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.mas_equalTo(200);
        make.height.mas_equalTo(44);
        make.left.mas_equalTo(self.brightnessLabel.mas_right).with.offset(5);
        make.centerY.equalTo(self.brightnessLabel);
    }];
    
    self.contrastLabel.text = @"contrast";
    self.contrastLabel.textColor = [UIColor whiteColor];
    [self.view addSubview:self.contrastLabel];
    [self.contrastLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.equalTo(self.contrastLabel.superview).with.offset(5);
        make.bottom.equalTo(self.brightnessLabel.mas_top).with.offset(-10);
    }];
    
    self.contrastSlider.minimumValue = 0.0;
    self.contrastSlider.maximumValue = 4.0;
    self.contrastSlider.value = 1.0;
    [self.contrastSlider addTarget:self
                            action:@selector(onContrastSlider:)
                  forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:self.contrastSlider];
    [self.contrastSlider mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.mas_equalTo(200);
        make.height.mas_equalTo(44);
        make.left.mas_equalTo(self.contrastLabel.mas_right).with.offset(5);
        make.centerY.equalTo(self.contrastLabel);
    }];
    
    self.saturationLabel.text = @"saturation";
    self.saturationLabel.textColor = [UIColor whiteColor];
    [self.view addSubview:self.saturationLabel];
    [self.saturationLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.equalTo(self.saturationLabel.superview).with.offset(5);
        make.bottom.equalTo(self.contrastLabel.mas_top).with.offset(-10);
    }];
    
    self.saturationSlider.minimumValue = 0.0;
    self.saturationSlider.maximumValue = 2.0;
    self.saturationSlider.value = 1.0;
    [self.saturationSlider addTarget:self
                            action:@selector(onSaturationSlider:)
                  forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:self.saturationSlider];
    [self.saturationSlider mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.mas_equalTo(200);
        make.height.mas_equalTo(44);
        make.left.mas_equalTo(self.saturationLabel.mas_right).with.offset(5);
        make.centerY.equalTo(self.saturationLabel);
    }];
}

- (void)setupFilter {
    _filterGroup = std::make_shared<TT::FilterGroup>();
    _filterGroup->addFilter([_imageView filter]);
    
    NSURL *movieURL = [[TTFileManager sharedInstance] newMovieURL];
    _movieWriter = [[TTMovieWriter alloc] initWithMovieURL:movieURL size:CGSizeMake(640, 480)];
    _filterGroup->addFilter([_movieWriter filter], 1);
    
    _saturationFilter = std::make_shared<TT::SaturationFilter>();
    _saturationFilter->addFilter(_filterGroup);
    
    _contrastFilter = std::make_shared<TT::ContrastFilter>();
    _contrastFilter->addFilter(_saturationFilter);
    
    _brightnessFilter = std::make_shared<TT::BrightnessFilter>();
    _brightnessFilter->addFilter(_contrastFilter);
}

- (void)setupCapture {
    self.capture.outputImageOrientation = AVCaptureVideoOrientationPortrait;
    self.capture.movieWriter = _movieWriter;
    [self.capture addFilter:_brightnessFilter];
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

- (TTCaptureButton *)captureButton {
    if (_captureButton == nil) {
        _captureButton = [TTCaptureButton new];
    }
    return _captureButton;
}

- (UILabel *)brightnessLabel {
    if (_brightnessLabel == nil) {
        _brightnessLabel = [UILabel new];
    }
    return _brightnessLabel;
}

- (UISlider *)brightnessSlider {
    if (_brightnessSlider == nil) {
        _brightnessSlider = [UISlider new];
    }
    return _brightnessSlider;
}

- (UILabel *)contrastLabel {
    if (_contrastLabel == nil) {
        _contrastLabel = [UILabel new];
    }
    return _contrastLabel;
}

- (UISlider *)contrastSlider {
    if (_contrastSlider == nil) {
        _contrastSlider = [UISlider new];
    }
    return _contrastSlider;
}

- (UILabel *)saturationLabel {
    if (_saturationLabel == nil) {
        _saturationLabel = [UILabel new];
    }
    return _saturationLabel;
}

- (UISlider *)saturationSlider {
    if (_saturationSlider == nil) {
        _saturationSlider = [UISlider new];
    }
    return _saturationSlider;
}

#pragma mark -
#pragma mark target/action

- (void)onClickCapture:(UIButton *)button {
    if (button.selected) {
        button.selected = NO;
        [self.capture stopCameraCapture];
    } else {
        button.selected = YES;
        [self.capture startCaptureVideo:YES andAudio:YES];
    }
}

- (void)onBrightnessSlider:(UISlider *)slider {
    NSLog(@"brightness %f", slider.value);
    _brightnessFilter->setBrightness(slider.value);
}

- (void)onContrastSlider:(UISlider *)slider {
    NSLog(@"contrast %f", slider.value);
    _contrastFilter->setContrast(slider.value);
}

- (void)onSaturationSlider:(UISlider *)slider {
    NSLog(@"saturation %f", slider.value);
    _saturationFilter->setSaturation(slider.value);
}

@end
