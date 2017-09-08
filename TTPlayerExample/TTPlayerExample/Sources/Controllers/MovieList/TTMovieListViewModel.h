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

- (TTMovieItemViewModel *)itemAtIndex:(NSUInteger)index;

@end
