//
//  TTEditViewController.h
//  TTPlayerExample
//
//  Created by liang on 21/1/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#import "TTBaseViewController.h"

@interface TTEditViewController : TTBaseViewController

@property (nonatomic, copy, readonly) NSArray<NSURL *> *urls;

- (instancetype)initWithURLs:(NSArray<NSURL *> *)urls;

@end
