//
//  TTMainViewController.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTMainViewController.h"

#import "TTMovieListViewController.h"
#import "TTCaptureViewController.h"

@interface TTMainViewController ()
{
    TTMovieListViewController *_movieListVC;
    TTCaptureViewController *_captureVC;
}

@end

@implementation TTMainViewController

- (instancetype)init {
    self = [super init];
    if (self) {
        _movieListVC = [TTMovieListViewController new];
        UINavigationController *movieListNav = [[UINavigationController alloc] initWithRootViewController:_movieListVC];
        movieListNav.tabBarItem.title = @"视频";
        
        _captureVC = [TTCaptureViewController new];
        UINavigationController *captureNav = [[UINavigationController alloc] initWithRootViewController:_captureVC];
        captureNav.tabBarItem.title = @"录制";
        
        self.viewControllers = @[movieListNav, captureNav];
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
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

@end
