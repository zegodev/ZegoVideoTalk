//
//  ZegoAdvancedTableViewController.m
//
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "ZegoAdvancedTableViewController.h"
#import "ZegoManager.h"
#import "ZegoSetting.h"

@interface ZegoAdvancedTableViewController () <UITextViewDelegate>

@property (weak, nonatomic) IBOutlet UISwitch *encodeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *decodeSwitch;

@property (weak, nonatomic) IBOutlet UISwitch *captureSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *renderSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *filterSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *audioPrepSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *timeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *rateControlSwitch;

@property (nonatomic, strong) UITapGestureRecognizer *tapGesture;

@end

@implementation ZegoAdvancedTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self updateUIView];
    
    UITapGestureRecognizer *gesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onAlphaEnv:)];
    gesture.numberOfTapsRequired = 5;
    [self.tableView addGestureRecognizer:gesture];
}

- (void)onAlphaEnv:(UIGestureRecognizer *)gesture
{
    BOOL alpha = [ZegoSetting sharedInstance].useTestEnv;
    [ZegoSetting sharedInstance].useAlphaEnv = !alpha;
    
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"测试环境"
                                                    message:alpha ? @"关闭Alpha环境" : @"打开Alpha环境"
                                                   delegate:nil
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil, nil];
    [alert show];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}

- (IBAction)toggleEncode:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].useHardwareEncode = s.on;
    [self updateUIView];
}

- (IBAction)toggleDecode:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].useHardwareDecode = s.on;
}

- (IBAction)toggleCapture:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].useExternalCapture = s.on;
}

- (IBAction)toggleRender:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].useExternalRender = s.on;
}

- (IBAction)toggleFilter:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].useExternalFilter = s.on;
}

- (IBAction)toggleRateControl:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].enableRateControl = s.on;
    [self updateUIView];
}

- (IBAction)toggleAudioPrep:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].enableAudioPrep = s.on;
}

- (IBAction)toggleTime:(id)sender
{
    UISwitch *s = (UISwitch *)sender;
    [ZegoSetting sharedInstance].recordTime = s.on;
    [self updateUIView];
}

- (void)updateUIView
{
    
#if TARGET_OS_SIMULATOR
    self.captureSwitch.enabled = NO;
#endif
    
    self.captureSwitch.on = [ZegoSetting sharedInstance].useExternalCapture;
    self.renderSwitch.on = [ZegoSetting sharedInstance].useExternalRender;
    self.filterSwitch.on =  [ZegoSetting sharedInstance].useExternalFilter;
    self.rateControlSwitch.on =  [ZegoSetting sharedInstance].enableRateControl;
    self.audioPrepSwitch.on =  [ZegoSetting sharedInstance].enableAudioPrep;
    self.timeSwitch.on =  [ZegoSetting sharedInstance].recordTime;
    self.encodeSwitch.on =  [ZegoSetting sharedInstance].useHardwareEncode;
    self.decodeSwitch.on =  [ZegoSetting sharedInstance].useHardwareDecode;
}

@end
