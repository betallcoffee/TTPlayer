//
//  TTCaptureButton.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import "TTCaptureButton.h"

@interface TTCaptureButton () <CAAnimationDelegate>

@property (nonatomic, strong) CAShapeLayer *borderLayer;
@property (nonatomic, strong) UIBezierPath *borderPath;

@property (nonatomic, strong) CAShapeLayer *colorLayer;
@property (nonatomic, strong) UIBezierPath *colorPath;

@property (nonatomic, strong) CAShapeLayer *maskLayer;
@property (nonatomic, strong) UIBezierPath *maskPath;

@end

@implementation TTCaptureButton

- (instancetype)init {
    self = [super init];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        
        _borderLayer = [CAShapeLayer layer];
        _borderLayer.backgroundColor = [UIColor clearColor].CGColor;
        _borderLayer.fillColor = [UIColor clearColor].CGColor;
        _borderLayer.strokeColor = [UIColor blackColor].CGColor;
        _borderLayer.lineWidth = 3;
        [self.layer addSublayer:_borderLayer];
        
        _colorLayer = [CAShapeLayer layer];
        _colorLayer.backgroundColor = [UIColor clearColor].CGColor;
        _colorLayer.fillColor = [UIColor redColor].CGColor;
        _colorLayer.strokeColor = [UIColor clearColor].CGColor;
        [self.layer addSublayer:_colorLayer];
        
        _maskLayer = [CAShapeLayer layer];
        _maskLayer.backgroundColor = [UIColor clearColor].CGColor;
        _maskLayer.fillColor = [UIColor redColor].CGColor;
        [self.layer addSublayer:_maskLayer];
    }
    
    return self;
}

- (void)layoutSubviews {
    self.borderLayer.frame = self.layer.bounds;
    self.colorLayer.frame = self.layer.bounds;
    self.maskLayer.frame = self.layer.bounds;
    
    CGPoint pos = CGPointMake(self.frame.size.width/2, self.frame.size.height/2);
    self.borderPath = [UIBezierPath bezierPathWithArcCenter:pos
                                                 radius:pos.x
                                             startAngle:0
                                               endAngle:M_PI * 2
                                              clockwise:YES];
    self.borderLayer.path = self.borderPath.CGPath;
    
    CGRect rect = CGRectMake(pos.x - pos.x/2, pos.y - pos.x/2, pos.x, pos.x);
    self.colorPath = [UIBezierPath bezierPathWithRect:rect];
    self.colorPath = [UIBezierPath bezierPathWithRoundedRect:rect cornerRadius:pos.x/4];
    self.colorLayer.path = self.colorPath.CGPath;
    
    if (self.maskPath == nil) {
        self.maskPath = [UIBezierPath bezierPathWithArcCenter:pos
                                                       radius:pos.x/2
                                                   startAngle:0
                                                     endAngle:M_PI * 2
                                                    clockwise:YES];
        self.maskLayer.path = self.maskPath.CGPath;
    }
}


// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}

#pragma mark -- 
#pragma mark setter/getter

- (void)setSelected:(BOOL)selected {
    [super setSelected:selected];
    if (selected) {
        CGPoint pos = CGPointMake(self.frame.size.width/2, self.frame.size.height/2);
        UIBezierPath *path = [UIBezierPath bezierPathWithArcCenter:pos
                                                            radius:pos.x-5
                                                        startAngle:0
                                                          endAngle:M_PI * 2
                                                         clockwise:YES];
        CABasicAnimation *anim = [CABasicAnimation animationWithKeyPath:@"path"];
        anim.duration = 0.2;
        anim.fromValue = (__bridge id)self.maskPath.CGPath;
        anim.toValue = (__bridge id)path.CGPath;
        self.maskPath = path;
        self.maskLayer.path = self.maskPath.CGPath;
        [self.maskLayer addAnimation:anim forKey:nil];
    } else {
        CGPoint pos = CGPointMake(self.frame.size.width/2, self.frame.size.height/2);
        UIBezierPath *path = [UIBezierPath bezierPathWithArcCenter:pos
                                                            radius:pos.x/2
                                                        startAngle:0
                                                          endAngle:M_PI * 2
                                                         clockwise:YES];
        CABasicAnimation *anim = [CABasicAnimation animationWithKeyPath:@"path"];
        anim.duration = 0.2;
        anim.fromValue = (__bridge id)self.maskPath.CGPath;
        anim.toValue = (__bridge id)path.CGPath;
        self.maskPath = path;
        self.maskLayer.path = self.maskPath.CGPath;
        [self.maskLayer addAnimation:anim forKey:nil];
    }
}

@end
