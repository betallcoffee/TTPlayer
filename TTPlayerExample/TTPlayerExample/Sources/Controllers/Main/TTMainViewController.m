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

#pragma mark --
#pragma mark UI
- (void)setupUI {
    [super setupUI];
    
//    self.interactive = YES;
    
    _movieListVC = [TTMovieListViewController new];
    _movieListVC.tabBarItem.tag = 0;
    _movieListVC.tabBarItem.title = @"视频";
    
    _captureVC = [TTCaptureViewController new];
    _captureVC.tabBarItem.tag = 1;
    _captureVC.tabBarItem.title = @"录制";
    
    self.tabBar.items = @[_movieListVC.tabBarItem, _captureVC.tabBarItem];
    self.tabBar.selectedItem = _movieListVC.tabBarItem;
    self.tabBar.itemPositioning = UITabBarItemPositioningFill;
    
    self.viewControllers = @[_movieListVC, _captureVC];

}

@end
