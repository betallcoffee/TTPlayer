//
//  TTMovieListViewModel.h
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTBaseViewModel.h"

@class TTMovieItemViewModel;

@interface TTMovieListViewModel : TTBaseViewModel

@property (nonatomic, readonly) NSUInteger total;
@property (nonatomic, strong, readonly) NSArray<NSURL *> *movieList;

- (TTMovieItemViewModel *)itemAtIndex:(NSUInteger)index;

@end
