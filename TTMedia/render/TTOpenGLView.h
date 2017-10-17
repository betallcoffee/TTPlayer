//
//  TTPlayerView.h
//  TTPlayerExample
//
//  Created by liang on 16/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "TTRender.hpp"
#include "TTFrame.hpp"

@interface TTOpenGLView : UIView

@property (nonatomic, assign) int sarNum;
@property (nonatomic, assign) int sarDen;

- (BOOL)setupRender:(TT::Render *)render;
- (BOOL)render:(std::shared_ptr<TT::Frame>)frame;

@end
