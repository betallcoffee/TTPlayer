//
//  main.m
//  TTPlayerExample
//
//  Created by liang on 9/4/15.
//  Copyright (c) 2015 tina. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    @try {
        @autoreleasepool {
            return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        }
    }
    @catch (NSException *exception) {
        NSLog(@"exception:%@", [exception callStackSymbols]);
    }
    @finally {
        
    }
   
}
