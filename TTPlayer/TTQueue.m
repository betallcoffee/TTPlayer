//
//  TTQueue.m
//  TTPlayerExample
//
//  Created by liang on 2/16/16.
//  Copyright © 2016 tina. All rights reserved.
//

#import "TTQueue.h"

@interface TTQueue<__covariant ObjectType> ()

@property (nonatomic, strong) NSMutableArray<ObjectType> *objectArray;
@property (nonatomic, strong) NSCondition *cond;
@property (nonatomic, assign) BOOL isClose;

@end

@implementation TTQueue

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.objectArray = [NSMutableArray new];
        self.cond = [NSCondition new];
        [self open];
    }
    return self;
}

- (void)dealloc
{
    [self close];
}

- (void)open
{
    [self.cond lock];
    self.isClose = NO;
    [self.cond unlock];
}

- (void)close
{
    [self.cond lock];
    self.isClose = YES;
    [self.cond broadcast];
    [self.cond unlock];
}

- (void)push:(id)object
{
    [self.cond lock];
    [self.objectArray addObject:object];
    [self.cond signal];
    [self.cond unlock];
}

- (id)pop
{
    [self.cond lock];
    while (self.objectArray.count <= 0 && !self.isClose) {
        [self.cond wait];
    }
    
    id object = [self.objectArray firstObject];
    [self.objectArray removeObject:object];

    [self.cond unlock];
    return  object;
}

- (id)first
{
    [self.cond lock];
    while (self.objectArray.count <= 0 && !self.isClose) {
        [self.cond wait];
    }
    
    id object = [self.objectArray firstObject];
    
    [self.cond unlock];
    return object;
}

@end
