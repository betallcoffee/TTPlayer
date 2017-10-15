//
//  TTTabViewController.h
//  TTPlayerExample
//
//  Created by liang on 8/9/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Masonry.h"

#import "TTBaseViewController.h"

@protocol TTTabViewControllerDelegate;

@interface TTTabViewController : TTBaseViewController

@property (nonatomic, weak, nullable) id<TTTabViewControllerDelegate> delegate;
@property (nonatomic, strong, readonly, nonnull) UITabBar *tabBar;
@property (nonatomic, assign) NSInteger selected;
@property (nonatomic, copy, nullable) NSArray<UIViewController *> *viewControllers;
@property (nonatomic, assign) BOOL interactive;
@property (nonatomic, strong) id<UIViewControllerInteractiveTransitioning> interactionController;

@end

@protocol TTTabViewControllerDelegate <NSObject>

@optional
/** Informs the delegate that the user selected view controller by tapping the corresponding icon.
 @note The method is called regardless of whether the selected view controller changed or not and only as a result of the user tapped a button. The method is not called when the view controller is changed programmatically. This is the same pattern as UITabBarController uses.
 */
- (void)tabViewController:(nonnull TTTabViewController *)tabViewController
  didSelectViewController:(nonnull UIViewController *)viewController;

/// Called on the delegate to obtain a UIViewControllerAnimatedTransitioning object which can be used to animate a non-interactive transition.
- (nullable id<UIViewControllerAnimatedTransitioning>)tabViewController:(nonnull TTTabViewController *)tabViewController animationControllerForTransitionFromViewController:(nonnull UIViewController *)fromVC
                                                       toViewController:(nonnull UIViewController *)toVC;

- (nullable id<UIViewControllerInteractiveTransitioning>)tabViewController:(nonnull TTTabViewController *)tabViewController
                               interactionControllerForAnimationController: (id <UIViewControllerAnimatedTransitioning>)animationController;

@end

/** A private UIViewControllerContextTransitioning class to be provided transitioning delegates.
 @discussion Because we are a custom UIVievController class, with our own containment implementation, we have to provide an object conforming to the UIViewControllerContextTransitioning protocol. The system view controllers use one provided by the framework, which we cannot configure, let alone create. This class will be used even if the developer provides their own transitioning objects.
 @note The only methods that will be called on objects of this class are the ones defined in the UIViewControllerContextTransitioning protocol. The rest is our own private implementation.
 */
@interface TTTabTransitionContext : NSObject <UIViewControllerContextTransitioning>

@property (nonatomic, weak, nullable) id<TTTabViewControllerDelegate> delegate;
@property (nonatomic, copy) void (^completionBlock)(BOOL didComplete); /// A block of code we can set to execute after having received the completeTransition: message.
@property (nonatomic, assign, getter=isAnimated) BOOL animated; /// Private setter for the animated property.
@property (nonatomic, assign, getter=isInteractive) BOOL interactive; /// Private setter for the interactive property.
@property (nonatomic, strong) id<UIViewControllerAnimatedTransitioning> animator;
@property (nonatomic, strong) id<UIViewControllerInteractiveTransitioning> interactionController;

- (instancetype)initWithFromViewController:(UIViewController *)fromVC toViewController:(UIViewController *)toVC goingRight:(BOOL)goingRight; /// Designated initializer.

- (void)startNoInteractiveTransition;

- (void)startInteractiveTransition;

- (void)activeInteractiveTransition;

@end

/** Instances of this private class perform the default transition animation which is to slide child views horizontally.
 @note The class only supports UIViewControllerAnimatedTransitioning at this point. Not UIViewControllerInteractiveTransitioning.
 */
@interface TTTabAnimatedTransition : NSObject <UIViewControllerAnimatedTransitioning>

@end

@interface TTTabInteractiveTransition : NSObject <UIViewControllerInteractiveTransitioning>

@end

