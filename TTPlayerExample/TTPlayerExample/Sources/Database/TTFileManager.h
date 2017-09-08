//
//  TTFileManager.h
//  TTPlayerExample
//
//  Created by liang on 7/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TTFileManager : NSObject

@property (nonatomic, copy) NSArray<NSURL *> *movieURLList;

+ (instancetype)sharedInstance;

- (NSURL *)newMovieURL;

@end
