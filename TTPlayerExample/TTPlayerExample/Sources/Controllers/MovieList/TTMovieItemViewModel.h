//
//  TTMovieItemViewModel.h
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTBaseViewModel.h"

@interface TTMovieItemViewModel : TTBaseViewModel

@property (nonatomic, assign, readonly) NSUInteger height;

@property (nonatomic, strong, readonly) NSString *name;
@property (nonatomic, readonly) NSURL *url;
@property (nonatomic, assign) NSNumber *size;

- (void)configData:(NSURL *)url;

@end
