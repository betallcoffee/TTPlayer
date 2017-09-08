//
//  TTMovieItemViewModel.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTMovieItemViewModel.h"

@interface TTMovieItemViewModel ()

@property (nonatomic, copy) NSURL *url;

@end

@implementation TTMovieItemViewModel

- (void)configData:(NSURL *)url {
    self.url = url;
}

#pragma mark -
#pragma mark setter/getter

- (NSUInteger)height {
    return 44;
}

- (NSString *)name {
    return self.url.lastPathComponent;
}

- (NSURL *)url {
    return _url;
}

@end
