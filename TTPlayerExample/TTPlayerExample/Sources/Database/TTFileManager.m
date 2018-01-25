//
//  TTFileManager.m
//  TTPlayerExample
//
//  Created by liang on 7/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTFileManager.h"

@interface TTFileManager ()
{
    NSFileManager *_fileManager;
    NSURL *_movieDir;
}

@property (nonatomic, strong, readonly) NSURL *movieDir;

@end

@implementation TTFileManager

+ (instancetype)sharedInstance {
    static TTFileManager *_sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _sharedInstance = [TTFileManager new];
        
    });
    return _sharedInstance;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _fileManager = [NSFileManager defaultManager];
    }
    return self;
}

- (NSURL *)newMovieURL {
    if (self.movieDir) {
        NSTimeInterval timestamp = [NSDate timeIntervalSinceReferenceDate];
        NSString *component = [NSString stringWithFormat:@"%lf.m4v", timestamp];
        NSURL *url = [self.movieDir URLByAppendingPathComponent:component];
        NSLog(@"TTFileManager new movie URL: %@", url);
        return url;
    }
    
    return nil;
}

#pragma mark -
#pragma setter/getter

- (NSArray<NSURL *> *)movieURLList {
    if (self.movieDir == nil) {
        return nil;
    } else if (_movieURLList && _movieURLList.count) {
        return [_movieURLList copy];
    } else {
        NSError *error;
        _movieURLList = [_fileManager contentsOfDirectoryAtURL:self.movieDir
                                    includingPropertiesForKeys:nil
                                                       options:NSDirectoryEnumerationSkipsHiddenFiles
                                                         error:&error];
        if (error) {
            NSLog(@"TTFileManager movie list error: %@", error);
        }
        return [_movieURLList copy];
    }
}

- (NSURL *)movieDir {
    if (_movieDir == nil) {
        NSArray<NSURL *> *dirs = [_fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
        NSURL *doc = dirs.firstObject;
        if (doc) {
            _movieDir = [doc URLByAppendingPathComponent:@"TTMovies" isDirectory:YES];
        }
        if (![_fileManager fileExistsAtPath:[_movieDir path]]) {
            NSError *error;
            [_fileManager createDirectoryAtURL:_movieDir
                   withIntermediateDirectories:YES
                                    attributes:nil
                                         error:&error];
            if (error) {
                NSLog(@"TTFileManager movie dir error: %@", error);
            }
        }
    }
    return _movieDir;
}

@end
