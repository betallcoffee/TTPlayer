//
//  TTQueue.h
//  TTPlayerExample
//
//  Created by liang on 2/16/16.
//  Copyright © 2016 tina. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TTQueue<__covariant ObjectType> : NSObject

- (void)open;
- (void)close;
- (void)push:(ObjectType)object;
- (ObjectType)pop;
- (ObjectType)first;

@end
