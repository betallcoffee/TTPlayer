//
//  TTCaptureButton.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTCaptureButton.h"

@interface TTCaptureButton () <CAAnimationDelegate>
{
    CALayer *_colorLayer;
}

@end

@implementation TTCaptureButton

- (instancetype)init {
    self = [super init];
    if (self) {
        _colorLayer = [CALayer layer];
        _colorLayer.backgroundColor = [UIColor yellowColor].CGColor;
//        [self.layer addSublayer:_colorLayer];
        [self addTarget:self
                 action:@selector(onClick:)
       forControlEvents:UIControlEventTouchUpInside];
    }
    
    return self;
}

- (void)layoutSubviews {
    _colorLayer.frame = self.bounds;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

- (void)onClick:(UIButton *)button {
    if (button.selected) {
        button.selected = NO;
        [self.layer removeAllAnimations];
    } else {
        button.selected = YES;
        CGFloat red = arc4random()/(CGFloat)INT_MAX;
        CGFloat green = arc4random()/(CGFloat)INT_MAX;
        CGFloat blue = arc4random()/(CGFloat)INT_MAX;
        UIColor *color = [UIColor colorWithRed:red
                                         green:green
                                          blue:blue
                                         alpha:1.0];
        
        CABasicAnimation *anim = [CABasicAnimation animation];
        anim.delegate = self;
        anim.duration = 3.0;
        anim.keyPath = @"backgroundColor";
        anim.toValue = (__bridge id)color.CGColor;
        CALayer *layer = self.layer.presentationLayer ? : self.layer.modelLayer;
        anim.fromValue = (__bridge id)layer.backgroundColor;
        anim.removedOnCompletion = NO;
        anim.fillMode = kCAFillModeForwards;

//        [CATransaction begin];
//        [CATransaction setDisableActions:YES];
//        [self.layer setValue:anim.toValue forKeyPath:anim.keyPath];
//        [CATransaction commit];
        
        [self.layer addAnimation:anim forKey:nil];
    }
}

- (void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag {
//    [CATransaction begin];
//    [CATransaction setDisableActions:YES];
//    CABasicAnimation *baseAnim = (CABasicAnimation *)anim;
//    [self.layer setValue:baseAnim.toValue forKeyPath:baseAnim.keyPath];
//    [CATransaction commit];
    NSLog(@"%@", self.layer.modelLayer.backgroundColor);
    NSLog(@"%@", self.layer.presentationLayer.backgroundColor);
    NSLog(@"%@", self.layer.backgroundColor);
}

@end
