//
//  TTEditViewController.m
//  TTPlayerExample
//
//  Created by liang on 21/1/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTEdit.hpp"

#import "TTEditViewController.h"

@interface TTEditViewController ()
{
    std::shared_ptr<TT::Edit> _edit;
    std::shared_ptr<TT::URL> _url;
}

@end

@implementation TTEditViewController

- (instancetype)initWithURL:(NSURL *)url {
    self = [super init];
    if (self) {
        const char *str = [url.absoluteString cStringUsingEncoding:NSUTF8StringEncoding];
        _url = std::make_shared<TT::URL>(str);
        _edit = std::make_shared<TT::Edit>();
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
