//
//  TTImageView.h
//  TTPlayerExample
//
//  Created by liang on 17/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "TTFilter_ios.h"

@interface TTImageView : UIView <TTFilterDelegate>

@property(readonly, nonatomic) CGSize sizeInPixels;

@end
