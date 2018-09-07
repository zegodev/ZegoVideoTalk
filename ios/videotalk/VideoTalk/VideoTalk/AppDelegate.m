//
//  AppDelegate.m
//  VideoTalk
//
//  Created by summery on 23/10/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "AppDelegate.h"
#import "ZegoHomeViewController.h"
#import "ZegoManager.h"
#import "ZegoSettingTableViewController.h"
#import <Bugly/Bugly.h>
#import "ZegoSetting.h"
#import <PgySDK/PgyManager.h>
#import <PgyUpdate/PgyUpdateManager.h>

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    // 初始化 sdk
    [ZegoManager api];
    
//    // 初始化 tencent qq sdk
//#if !TARGET_OS_SIMULATOR
//    (void)[[TencentOAuth alloc] initWithAppId:@"1106489612" andDelegate:nil];
//#endif
    
    // 初始化 bugly
    [self setupBugly];
    
    [self setupPGY];
    
    // 初始化主界面
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.backgroundColor = [UIColor whiteColor];
    
    ZegoHomeViewController *homeController = [[ZegoHomeViewController alloc] initWithNibName:@"ZegoHomeViewController" bundle:nil];
    
    UIBarButtonItem *right = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"设置", nil) style:UIBarButtonItemStylePlain target:self action:@selector(showSetting:)];
    [homeController.navigationItem setRightBarButtonItem:right];
    NSString *title = [NSString stringWithFormat:@"ZEGO(%@)", [ZegoSetting sharedInstance].appTypeList[[ZegoSetting sharedInstance].appType]];
    [homeController.navigationItem setTitle:title];
    
    self.navigationController = [[UINavigationController alloc] initWithRootViewController:homeController];
    
    self.window.rootViewController = self.navigationController;
    
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)showSetting:(id)sender {
    UIStoryboard *sb = [UIStoryboard storyboardWithName:@"ZegoSettingTableViewController" bundle:nil];
    ZegoSettingTableViewController *settingController = [sb instantiateViewControllerWithIdentifier:@"Setting"];
    [settingController.navigationItem setTitle: NSLocalizedString(@"设置", nil)];
    
    [self.navigationController pushViewController:settingController animated:YES];
}

- (void)setupPGY {
    [[PgyUpdateManager sharedPgyManager] startManagerWithAppId:@"18bd9c212c59b199b849f9bc09851c2e"];   // 请将 PGY_APP_ID 换成应用的 App Key
    [[PgyUpdateManager sharedPgyManager] checkUpdate];
}

- (void)setupBugly {
    // Get the default config
    BuglyConfig * config = [[BuglyConfig alloc] init];
    
    // Open the debug mode to print the sdk log message.
    // Default value is NO, please DISABLE it in your RELEASE version.
#if DEBUG
    config.debugMode = YES;
#endif
    
    // Open the customized log record and report, BuglyLogLevelWarn will report Warn, Error log message.
    // Default value is BuglyLogLevelSilent that means DISABLE it.
    // You could change the value according to you need.
    config.reportLogLevel = BuglyLogLevelWarn;
    
    // Open the STUCK scene data in MAIN thread record and report.
    // Default value is NO
    config.blockMonitorEnable = YES;
    
    // Set the STUCK THRESHOLD time, when STUCK time > THRESHOLD it will record an event and report data when the app launched next time.
    // Default value is 3.5 second.
    config.blockMonitorTimeout = 1.5;
    
    // Set the app channel to deployment
    config.channel = @"Bugly";
    
    // NOTE:Required
    // Start the Bugly sdk with APP_ID and your config
    [Bugly startWithAppId:@"ba7406f54a"
#if DEBUG
        developmentDevice:YES
#endif
                   config:config];
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
