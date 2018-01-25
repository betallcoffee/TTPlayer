//
//  TTPlayer_ios.h
//  TTPlayerExample
//
//  Created by liang on 19/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTPlayer_ios_h
#define TTPlayer_ios_h

#include "TTPlayer.hpp"
#include "TTRender.hpp"

#import "TTOpenGLView.h"

TT::Player *createPlayer_ios();

BOOL bindGLView_ios(TT::Player *player, TTOpenGLView *view);


#endif /* TTPlayer_ios_h */
