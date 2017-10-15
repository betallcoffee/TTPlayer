//
//  TTBaseViewController.m
//  TTPlayerExample
//
//  Created by liang on 31/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTBaseViewController.h"

@interface TTBaseViewController ()

@end

@implementation TTBaseViewController

- (instancetype) init {
    self = [super init];
    if (self) {
        
    }
    NSLog(@"%@:%@", self.class, NSStringFromSelector(_cmd));
    return self;
}

- (void)dealloc {
    NSLog(@"%@:%@", self.class, NSStringFromSelector(_cmd));
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [self setupUI];
    
    [self configViewModel];
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

}

#pragma mark -
#pragma mark ViewModel

- (void)configViewModel {
    
}

@end
