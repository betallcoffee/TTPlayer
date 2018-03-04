//
//  TTMovieListViewModel.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTMovieListViewModel.h"
#import "TTMovieItemViewModel.h"
#import "TTFileManager.h"

@interface TTMovieListViewModel ()
{
    NSMutableArray<NSURL *> *_movieList;
}

@end

@implementation TTMovieListViewModel

- (TTMovieItemViewModel *)itemAtIndex:(NSUInteger)index {
    TTMovieItemViewModel *item = [TTMovieItemViewModel new];
    NSURL *url = [self.movieList objectAtIndex:index];
    NSDictionary *attr = [[NSFileManager defaultManager] attributesOfItemAtPath:url.path error:nil];
    NSLog(@"url: %@", url);
    NSLog(@"attr: %@", attr);
    [item configData:url];
    return item;
}

#pragma mark -
#pragma setter/getter

- (NSUInteger)total {
    return self.movieList.count;
}

- (NSArray<NSURL *> *)movieList {
    if (_movieList == nil) {
        _movieList = [NSMutableArray new];
        [_movieList addObjectsFromArray:[TTFileManager sharedInstance].movieURLList];
    }
    return _movieList;
}

@end
