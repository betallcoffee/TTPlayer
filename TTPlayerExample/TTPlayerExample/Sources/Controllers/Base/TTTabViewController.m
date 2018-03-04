//
//  TTBaseTabViewController.m
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//
// http://blog.devtang.com/2016/03/13/iOS-transition-guide/
#import "TTTabViewController.h"

@interface TTTabViewController ()<UITabBarDelegate>

@property (nonatomic, strong, readwrite) UITabBar *tabBar;
@property (nonatomic, strong) UIView *containerView;

@end

@implementation TTTabViewController

- (instancetype)init {
    self = [super init];
    if (self) {
        _selected = -1;
        _interactive = NO;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

#pragma mark --
#pragma mark UI
- (void)setupUI {
    [super setupUI];
    
    self.tabBar.delegate = self;
    self.tabBar.backgroundColor = [UIColor whiteColor];
    [self.view addSubview:self.tabBar];
    [self.tabBar mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.equalTo(self.tabBar.superview);
        make.right.equalTo(self.tabBar.superview);
        make.bottom.equalTo(self.tabBar.superview);
        make.height.mas_equalTo(44);
    }];
    
    [self.view addSubview:self.containerView];
    [self.containerView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.equalTo(self.containerView.superview);
        make.top.equalTo(self.containerView.superview).offset(20);
        make.bottom.equalTo(self.tabBar.mas_top);
    }];
}

#pragma mark --
#pragma mark Transitioning

- (void)transitionFromIndex:(NSInteger)fromIndex
                     toIndex:(NSInteger)toIndex {
    if (fromIndex == toIndex) {
        return ;
    }
    
    UIViewController *fromVC = nil;
    UIViewController *toVC = nil;
    if (fromIndex < self.viewControllers.count) {
        fromVC = self.viewControllers[fromIndex];
    }
    
    if (toIndex < self.viewControllers.count) {
        toVC = self.viewControllers[toIndex];
    }
    if (toVC == nil) {
        return ;
    }
    
    [fromVC willMoveToParentViewController:nil];
    [self addChildViewController:toVC];
    
    if (fromVC == nil) {
        [self.containerView addSubview:toVC.view];
        [toVC.view mas_makeConstraints:^(MASConstraintMaker *make) {
            make.edges.equalTo(toVC.view.superview);
        }];
        [toVC didMoveToParentViewController:self];
        return ;
    }
    
    // Because of the nature of our view controller, with horizontally arranged buttons, we instantiate our private transition context with information about whether this is a left-to-right or right-to-left transition. The animator can use this information if it wants.
    TTTabTransitionContext *transitionContext = [[TTTabTransitionContext alloc] initWithFromViewController:fromVC toViewController:toVC goingRight:toIndex > fromIndex];
    
    transitionContext.delegate = self.delegate;
    transitionContext.animated = YES;
    transitionContext.interactive = self.interactive;
    transitionContext.completionBlock = ^(BOOL didComplete) {
        [fromVC.view removeFromSuperview];
        [fromVC removeFromParentViewController];
        [toVC didMoveToParentViewController:self];
        self.tabBar.userInteractionEnabled = YES;
    };
    
    id<UIViewControllerAnimatedTransitioning> animator = nil;
    if ([self.delegate respondsToSelector:@selector(tabViewController:animationControllerForTransitionFromViewController:toViewController:)]) {
        animator = [self.delegate tabViewController:self animationControllerForTransitionFromViewController:fromVC toViewController:toVC];
    }
    transitionContext.animator = animator;
    
    id<UIViewControllerInteractiveTransitioning> interactionController = nil;
    if ([self.delegate respondsToSelector:@selector(tabViewController:interactionControllerForAnimationController:)]) {
        interactionController = [self.delegate tabViewController:self interactionControllerForAnimationController:animator];
    }
    transitionContext.interactionController = interactionController;
    
    if (self.interactive) {
        [transitionContext startInteractiveTransition];
    } else {
        self.tabBar.userInteractionEnabled = NO;
        [transitionContext startNoInteractiveTransition];
    }
}

#pragma mark --
#pragma mark UITabBarDelegate

- (void)tabBar:(UITabBar *)tabBar didSelectItem:(UITabBarItem *)item {
    self.selected = item.tag;
}

#pragma mark --
#pragma mark setter/getter

- (UITabBar *)tabBar {
    if (_tabBar == nil) {
        _tabBar = [UITabBar new];
    }
    return _tabBar;
}

- (UIView *)containerView {
    if (_containerView == nil) {
        _containerView = [UIView new];
    }
    return _containerView;
}

- (void)setViewControllers:(NSArray<UIViewController *> *)viewControllers {
    for (UIViewController *vc in _viewControllers) {
        [vc.view removeFromSuperview];
        [vc removeFromParentViewController];
    }
    _viewControllers = [viewControllers copy];
    self.selected = 0;
}

- (void)setSelected:(NSInteger)selected {
    if (selected < self.viewControllers.count) {
        NSInteger fromIndex = _selected;
        _selected = selected;
        [self transitionFromIndex:fromIndex toIndex:_selected];
    }
}

@end

#pragma mark --
#pragma mark UIViewControllerContextTransitioning

@interface TTTabTransitionContext ()

@property (nonatomic, strong) NSDictionary *privateViewControllers;
@property (nonatomic, assign) CGRect privateDisappearingFromRect;
@property (nonatomic, assign) CGRect privateAppearingFromRect;
@property (nonatomic, assign) CGRect privateDisappearingToRect;
@property (nonatomic, assign) CGRect privateAppearingToRect;
@property (nonatomic, weak) UIView *containerView;
@property (nonatomic, assign) UIModalPresentationStyle presentationStyle;

@end

@implementation TTTabTransitionContext

- (instancetype)initWithFromViewController:(UIViewController *)fromVC
                          toViewController:(UIViewController *)toVC
                                goingRight:(BOOL)goingRight {
    NSAssert ([fromVC isViewLoaded] && fromVC.view.superview, @"The fromViewController view must reside in the container view upon initializing the transition context.");
    
    if ((self = [super init])) {
        self.presentationStyle = UIModalPresentationCustom;
        self.containerView = fromVC.view.superview;
        self.privateViewControllers = @{
                                        UITransitionContextFromViewControllerKey:fromVC,
                                        UITransitionContextToViewControllerKey:toVC,
                                        };
        
        // Set the view frame properties which make sense in our specialized ContainerViewController context. Views appear from and disappear to the sides, corresponding to where the icon buttons are positioned. So tapping a button to the right of the currently selected, makes the view disappear to the left and the new view appear from the right. The animator object can choose to use this to determine whether the transition should be going left to right, or right to left, for example.
        CGFloat travelDistance = (goingRight ? -self.containerView.bounds.size.width : self.containerView.bounds.size.width);
        self.privateDisappearingFromRect = self.privateAppearingToRect = self.containerView.bounds;
        self.privateDisappearingToRect = CGRectOffset (self.containerView.bounds, travelDistance, 0);
        self.privateAppearingFromRect = CGRectOffset (self.containerView.bounds, -travelDistance, 0);
    }
    
    return self;
}

- (CGRect)initialFrameForViewController:(UIViewController *)viewController {
    if (viewController == [self viewControllerForKey:UITransitionContextFromViewControllerKey]) {
        return self.privateDisappearingFromRect;
    } else {
        return self.privateAppearingFromRect;
    }
}

- (CGRect)finalFrameForViewController:(UIViewController *)viewController {
    if (viewController == [self viewControllerForKey:UITransitionContextFromViewControllerKey]) {
        return self.privateDisappearingToRect;
    } else {
        return self.privateAppearingToRect;
    }
}

- (UIViewController *)viewControllerForKey:(NSString *)key {
    return self.privateViewControllers[key];
}

- (void)completeTransition:(BOOL)didComplete {
    if ([self.animator respondsToSelector:@selector (animationEnded:)]) {
        [self.animator animationEnded:didComplete];
    }
    if (self.completionBlock) {
        self.completionBlock (didComplete);
    }
}

- (BOOL)transitionWasCancelled { return NO; } // Our non-interactive transition can't be cancelled (it could be interrupted, though)

- (void)startNoInteractiveTransition {
    // Animate the transition by calling the animator with our private transition context. If we don't have a delegate, or if it doesn't return an animated transitioning object, we will use our own, private animator.
    self.animator = (self.animator ?: [TTTabAnimatedTransition new]);
    [self.animator animateTransition:self];
}

- (void)startInteractiveTransition {
    self.interactionController = (self.interactionController ?: [TTTabInteractiveTransition new]);
    [self.interactionController startInteractiveTransition:self];
}

- (void)activeInteractiveTransition {
    self.containerView.layer.speed = 0;
    [self.animator animateTransition:self];
}

// Supress warnings by implementing empty interaction methods for the remainder of the protocol:

- (void)updateInteractiveTransition:(CGFloat)percentComplete {}
- (void)finishInteractiveTransition {}
- (void)cancelInteractiveTransition {}

@end

#pragma mark --
#pragma mark UIViewControllerAnimatedTransitioning

@implementation TTTabAnimatedTransition

static CGFloat const kChildViewPadding = 16;
static CGFloat const kDamping = 0.75;
static CGFloat const kInitialSpringVelocity = 0.5;

- (NSTimeInterval)transitionDuration:(id<UIViewControllerContextTransitioning>)transitionContext {
    return 1;
}

- (void)animateTransition:(id <UIViewControllerContextTransitioning>)transitionContext {
    UIViewController* toVC = [transitionContext viewControllerForKey:UITransitionContextToViewControllerKey];
    UIViewController* fromVC = [transitionContext viewControllerForKey:UITransitionContextFromViewControllerKey];
    
    // When sliding the views horizontally in and out, figure out whether we are going left or right.
    BOOL goingRight = ([transitionContext initialFrameForViewController:toVC].origin.x < [transitionContext finalFrameForViewController:toVC].origin.x);
    CGFloat travelDistance = [transitionContext containerView].bounds.size.width + kChildViewPadding;
    CGAffineTransform travel = CGAffineTransformMakeTranslation (goingRight ? travelDistance : -travelDistance, 0);
    
    [[transitionContext containerView] addSubview:toVC.view];
    toVC.view.alpha = 0;
    toVC.view.transform = CGAffineTransformInvert (travel);
    [toVC.view mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(toVC.view.superview);
    }];
    
    [UIView animateWithDuration:[self transitionDuration:transitionContext] delay:0 usingSpringWithDamping:kDamping initialSpringVelocity:kInitialSpringVelocity options:0x00 animations:^{
        fromVC.view.transform = travel;
        fromVC.view.alpha = 0;
        toVC.view.transform = CGAffineTransformIdentity;
        toVC.view.alpha = 1;
    } completion:^(BOOL finished) {
        fromVC.view.transform = CGAffineTransformIdentity;
        [transitionContext completeTransition:![transitionContext transitionWasCancelled]];
    }];
}

@end

#pragma mark --
#pragma mark UIViewControllerInteractiveTransitioning

@interface TTTabInteractiveTransition ()

@property (nonatomic, weak) TTTabTransitionContext *transitionContext;

@end

@implementation TTTabInteractiveTransition

- (void)startInteractiveTransition:(id <UIViewControllerContextTransitioning>)transitionContext {
    self.transitionContext = transitionContext;
    [self.transitionContext activeInteractiveTransition];
}

- (void)updateInteractiveTransition:(CGFloat)percentComplete {
    [self.transitionContext updateInteractiveTransition:percentComplete];
}

- (void)cancelInteractiveTransition {
    [self.transitionContext cancelInteractiveTransition];
}

- (void)finishInteractiveTransition {
    [self.transitionContext finishInteractiveTransition];
}

@end

