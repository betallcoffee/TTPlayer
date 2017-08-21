//
//  TTMovieWriter.h
//  TTPlayerExample
//
//  Created by liang on 21/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "TTFilter_ios.h"

@interface TTMovieWriter : TTFilter

- (id)initWithMovieURL:(NSURL *)newMovieURL size:(CGSize)newSize;
- (id)initWithMovieURL:(NSURL *)newMovieURL size:(CGSize)newSize fileType:(NSString *)newFileType outputSettings:(NSDictionary *)outputSettings;

- (void)start;
- (void)finish;
- (void)cancel;

@end
