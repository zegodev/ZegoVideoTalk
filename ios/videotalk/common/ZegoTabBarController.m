//
//  ZegoTabBarController.m
//  
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "ZegoTabBarController.h"
#import "ZegoSingleAnchorViewController.h"
#import "ZegoSetting.h"
#import "ZegoPublishViewController.h"
#import "ZegoRoomViewController.h"
#import "ZegoManager.h"

#import <TencentOpenAPI/QQApiInterface.h>
#import <TencentOpenAPI/QQApiInterfaceObject.h>

@interface ZegoTabBarController() <UITabBarDelegate, ZegoRoomViewControllerDelegate>

@end

@implementation ZegoTabBarController

#pragma mark - Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // 设置 leftBarButtonItem 为 联系我们 button
    [self setLeftBarButtonItemContactUs];

    // 设置 rightBarButtonItem 为刷新 button
    [self setRightBarButtonItemTitle];
    
    // 设置 navigationItem 为 ZEGO(xxx)
    NSString *title = [NSString stringWithFormat:@"ZEGO(%@)", [ZegoSetting sharedInstance].appTypeList[[ZegoSetting sharedInstance].appType]];
    [self setViewControllerTitle:NSLocalizedString(title, nil)];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    for (UIViewController *viewController in self.viewControllers) {
        if ([viewController isKindOfClass:[ZegoRoomViewController class]]) {
            ZegoRoomViewController *room = (ZegoRoomViewController *)viewController;
            room.delegate = self;
        }
    }
}

#pragma mark - Event response

- (void)onRightBarButton:(id)sender {
    [self setRightBarButtonItemActivityView];  // 点击后开始刷新，刷新时 rightBarButtonItem 转菊花
    
    UIViewController *viewController = self.selectedViewController;
    if ([viewController isKindOfClass:[ZegoRoomViewController class]]) {
        ZegoRoomViewController *room = (ZegoRoomViewController *)viewController;
        [room refreshRoomList];
    }
}

// 联系我们
- (void)onContactUs:(id)sender {
#if TARGET_OS_SIMULATOR
#else
    if (![QQApiInterface isQQInstalled])
    {
        if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0) {
            // 兼容 iOS 8.0 及以下系统版本
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"没有安装 QQ", nil)
                                                                message:nil
                                                               delegate:self
                                                      cancelButtonTitle:nil
                                                      otherButtonTitles:NSLocalizedString(@"OK", nil), nil];
            [alertView show];
        } else {
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"没有安装 QQ", nil)
                                                                                     message:nil
                                                                              preferredStyle:UIAlertControllerStyleAlert];
            UIAlertAction *confirm = [UIAlertAction actionWithTitle:NSLocalizedString(@"OK", nil)
                                                              style:UIAlertActionStyleDefault
                                                            handler:^(UIAlertAction * _Nonnull action) {
                                                                
                                                            }];
            [alertController addAction:confirm];
            
            [self presentViewController:alertController animated:YES completion:nil];
        }
    }
    
    QQApiWPAObject *wpaObject = [QQApiWPAObject objectWithUin:@"84328558"];
    SendMessageToQQReq *req = [SendMessageToQQReq reqWithContent:wpaObject];
    QQApiSendResultCode result = [QQApiInterface sendReq:req];
    NSLog(@"share result %d", result);
#endif
}

#pragma mark - Private

// 设置 leftBarButtonItem 为联系我们 button
- (void)setLeftBarButtonItemContactUs {
    UIBarButtonItem *leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"联系我们", nil)
                                                                          style:UIBarButtonItemStylePlain
                                                                         target:self
                                                                         action:@selector(onContactUs:)];
    self.navigationItem.leftBarButtonItem = leftBarButtonItem;
}

// 设置 rightBarButtonItem 为刷新 button
- (void)setRightBarButtonItemTitle {
    UIBarButtonItem *rightButtonItem = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"刷新", nil)
                                                                        style:UIBarButtonItemStylePlain
                                                                       target:self
                                                                       action:@selector(onRightBarButton:)];
    self.navigationItem.rightBarButtonItem = rightButtonItem;
}

// 自定义 rightBarButtonItem 为转菊花 view
- (void)setRightBarButtonItemActivityView {
    UIActivityIndicatorView *activityView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    activityView.hidesWhenStopped = YES;
    [activityView startAnimating];
    
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:activityView];
}


#pragma mark - Public

- (void)setViewControllerTitle:(NSString *)title {
    self.navigationItem.title = title;
}

#pragma mark - UITabBarDelegate

// 选中直播列表 tab 时
- (void)tabBar:(UITabBar *)tabBar didSelectItem:(UITabBarItem *)item {
    // 只有房间界面右上角显示刷新，其他界面不显示
    if ([tabBar.items indexOfObject:item] == 0) {
        [self setRightBarButtonItemTitle];
    } else {
        self.navigationItem.rightBarButtonItem = nil;
    }
}

#pragma mark - ZegoRoomViewControllerDelegate

// 刷新结束后，转菊花停止
- (void)onRefreshRoomListFinished {
    if (self.navigationItem.rightBarButtonItem != nil) {
        [self setRightBarButtonItemTitle];
    }
}

@end
