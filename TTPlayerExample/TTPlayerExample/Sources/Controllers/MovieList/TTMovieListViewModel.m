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

}

@property (nonatomic, strong, readonly) NSArray<NSURL *> *movieList;

@end

@implementation TTMovieListViewModel

- (TTMovieItemViewModel *)itemAtIndex:(NSUInteger)index {
    TTMovieItemViewModel *item = [TTMovieItemViewModel new];
    NSURL *url = [[TTFileManager sharedInstance].movieURLList objectAtIndex:index];
    [item configData:url];
    return item;
}

#pragma mark -
#pragma setter/getter

- (NSUInteger)total {
    return self.movieList.count;
}

- (NSArray<NSURL *> *)movieList {
    return [TTFileManager sharedInstance].movieURLList;
}

@end
