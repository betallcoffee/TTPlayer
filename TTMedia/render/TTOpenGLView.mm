//
//  TTPlayerView.m
//  TTPlayerExample
//
//  Created by liang on 16/7/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#include "easylogging++.h"
#import "TTOpenGLView.h"

@interface TTOpenGLView ()

@property (nonatomic, assign) TT::Render *render;
@property (nonatomic, assign) BOOL isSetupRender;
@property (nonatomic, strong) NSArray *observers;
@property (nonatomic, strong) NSRecursiveLock *lock;
@property (nonatomic, assign) BOOL isActive;
@property (nonatomic, strong) EAGLContext *context;
@property (nonatomic, assign) CGSize size;

@end

@implementation TTOpenGLView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (instancetype)init {
    if (self = [super init]) {
        _sarNum = 0;
        _sarDen = 1;
        
        CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.contentsScale = [UIScreen mainScreen].scale;
        eaglLayer.drawableProperties = @{kEAGLDrawablePropertyRetainedBacking: @(NO) ,
                                         kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8};
        
        _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!_context || ![EAGLContext setCurrentContext:_context]) {
            return nil;
        }
        
        _lock = [NSRecursiveLock new];
        _isActive = (UIApplicationStateActive == [UIApplication sharedApplication].applicationState);
        
        __weak typeof(self) wself = self;
        id observer1 = [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidEnterBackgroundNotification object:nil queue:nil usingBlock:^(NSNotification *notif) {
            __strong typeof(wself) strongSelf = wself;
        }];
        id observer2 = [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notif) {
            __strong typeof(wself) strongSelf = wself;
        }];
        
        self.observers = @[observer1, observer2];
        
    }
    
    return self;
}

- (void)dealloc {
    [self.observers enumerateObjectsUsingBlock:^(id observer, NSUInteger idx, BOOL *stop) {
        [[NSNotificationCenter defaultCenter] removeObserver:observer];
    }];
    self.observers = nil;

    if ([EAGLContext currentContext] == _context) {
        [EAGLContext setCurrentContext:nil];
    }
    _context = nil;
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    [self.lock lock];
    if (_render) {
        [EAGLContext setCurrentContext:_context];
        _render->bindRenderBuffer();
        [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        _render->attachRenderBuffer();
        _render->updateBuffers(_sarNum, _sarDen, _size.width, _size.height);
    }
    
    [self.lock unlock];
}

- (BOOL)setupRender:(TT::Render *)render {
    BOOL bRet = NO;
    [self.lock lock];
    _render = render;
    if (self.isSetupRender) {
        bRet = YES;
    } else if (_render && !self.isSetupRender) {
        [EAGLContext setCurrentContext:_context];
        do {
            if (!_render->createFrameBuffer()) break;
            if (!_render->createRenderBuffer()) break;
            if (!_render->bindRenderBuffer()) break;
            [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
            if (!_render->attachRenderBuffer()) break;
        
            if (!_render->loadShader()) break;
            
            bRet = YES;
        } while(NO);
        
        self.isSetupRender = bRet;
        
        NSLog(@"setupRender result: %d", self.isSetupRender);
    }
    
    [self.lock unlock];
    
    return bRet;
}

- (BOOL)render:(std::shared_ptr<TT::Frame>)frame {
    if (frame == nullptr || _render == nullptr) {
        return NO;
    }
    
    TIMED_FUNC(timed);
    [self.lock lock];
    [EAGLContext setCurrentContext:_context];
    
    if (_size.width != frame->width || _size.height != frame->height) {
        _size = CGSizeMake(frame->width, frame->height);
        _render->updateBuffers(_sarNum, _sarDen, frame->width, frame->height);
    }
    
    PERFORMANCE_CHECKPOINT(timed);
    
    _render->uploadTexture(frame);
    _render->bindRenderBuffer();
    
    PERFORMANCE_CHECKPOINT(timed);
    [_context presentRenderbuffer:GL_RENDERBUFFER];
    PERFORMANCE_CHECKPOINT(timed);
    
    [self.lock unlock];
    return YES;
}

@end
