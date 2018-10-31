//
//  ZegoPublishViewController.m
//  
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "ZegoPublishViewController.h"
#import "ZegoSingleAnchorViewController.h"
#import "ZegoMultiAnchorViewController.h"
#import "ZegoMixStreamAnchorViewController.h"
#import "ZegoSetting.h"
#import "ZegoManager.h"

#define MAX_TITLE_LENGTH    30

@interface ZegoPublishViewController () <UIPickerViewDelegate, UIPickerViewDataSource, ZegoDeviceEventDelegate>

@property (weak, nonatomic) IBOutlet UIView *presetView;            // 设置空间的父 view
@property (weak, nonatomic) IBOutlet UISwitch *cameraSwitch;        // 切换摄像头
@property (weak, nonatomic) IBOutlet UISwitch *torchSwitch;         // 手电筒
@property (weak, nonatomic) IBOutlet UIPickerView *beautifyPicker;   // 美颜
@property (weak, nonatomic) IBOutlet UIPickerView *filterPicker;    // 滤镜
@property (weak, nonatomic) IBOutlet UITextField *titleField;       // 直播标题
@property (weak, nonatomic) IBOutlet UIButton *publishButton;       // 开始直播

@property (nonatomic, strong) UIView *previewView;                  // 预览 view

@property (nonatomic, strong) UIImageView *videoView;
@property (nonatomic, strong) NSTimer *previewTimer;

@end

@implementation ZegoPublishViewController

#pragma mark - Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [[ZegoManager api] setDeviceEventDelegate:self];
    self.beautifyPicker.delegate = self;
    self.beautifyPicker.dataSource = self;
    self.filterPicker.delegate = self;
    self.filterPicker.dataSource = self;
    
    self.cameraSwitch.on = YES;
    self.torchSwitch.on = NO;
    self.torchSwitch.enabled = NO;
    
    self.presetView.backgroundColor = [UIColor clearColor];
    self.publishButton.layer.cornerRadius = 4.0f;
    
    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapView:)];
    [self.presetView addGestureRecognizer:tapGesture];
    
    [self addPreviewView];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    NSInteger row = 0;
    switch ([ZegoSetting sharedInstance].beautifyFeature) {
        case ZEGO_BEAUTIFY_POLISH:
            row = 1;
            break;
        case ZEGO_BEAUTIFY_WHITEN:
            row = 2;
            break;
        case ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_WHITEN:
            row = 3;
            break;
        case ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_SKINWHITEN:
            row = 4;
            break;
            
        default:
            break;
    }
    
    [self.beautifyPicker selectRow:row inComponent:0 animated:NO];
    [self.filterPicker selectRow:[ZegoSetting sharedInstance].filterFeature inComponent:0 animated:NO];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationDeactive:) name:UIApplicationWillResignActiveNotification object:nil];
    
    if (self.previewView == nil) {
        [self addPreviewView];
    }
    
    BOOL videoAuthorization = [self checkVideoAuthorization];
    BOOL audioAuthorization = [self checkAudioAuthorization];
    
    if (videoAuthorization == YES)
    {
        [self startPreview];
        [[ZegoManager api] setAppOrientation:[UIApplication sharedApplication].statusBarOrientation];
        
        if (audioAuthorization == NO)
        {
            [self showAlert:NSLocalizedString(@"请点击设置，开启 VideoLive 的麦克风权限", nil) title:NSLocalizedString(@"VideoLive 需要访问麦克风", nil)];
        }
    }
    else
    {
        [self showAlert:NSLocalizedString(@"请点击设置，开启 VideoLive 的相机权限", nil) title:NSLocalizedString(@"VideoLive 需要访问相机", nil)];
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    if (self.previewView) {
        [self stopPreview];
    }
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super viewWillDisappear:animated];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Event response

- (void)onTapView:(UIGestureRecognizer *)recognizer {
    [self.titleField resignFirstResponder];
}

- (void)onApplicationActive:(NSNotification *)notification {
    if (self.tabBarController.selectedIndex == 1 && self.presentedViewController == nil && self.previewView != nil) {
        [self stopPreview];
        [self startPreview];
    }
}

- (void)onApplicationDeactive:(NSNotification *)notification {
    if (self.tabBarController.selectedIndex == 1 && self.presentedViewController == nil) {
        [self stopPreview];
    }
}

- (IBAction)onSwitchCamera:(id)sender {
    [self.titleField resignFirstResponder];
    
    [[ZegoManager api] setFrontCam:self.cameraSwitch.on];
    if (self.cameraSwitch.on) {
        self.torchSwitch.enabled = NO;  // 使用前置摄像头时不能开手电筒
    } else {
        self.torchSwitch.enabled = YES;
    }
}

- (IBAction)onSwitchTorch:(id)sender {
    [self.titleField resignFirstResponder];
    
    [[ZegoManager api] enableTorch:self.torchSwitch.on];
}

- (IBAction)onStartPublish:(id)sender {
    if ([self isDeviceiOS7]) {
        
    } else {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil
                                                                                 message:NSLocalizedString(@"请选择直播模式", nil)
                                                                          preferredStyle:UIAlertControllerStyleActionSheet];
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:NSLocalizedString(@"取消", nil)
                                                         style:UIAlertActionStyleCancel
                                                       handler:nil];
        UIAlertAction *singleAnchor = [UIAlertAction actionWithTitle:NSLocalizedString(@"单主播模式", nil)
                                                              style:UIAlertActionStyleDefault
                                                            handler:^(UIAlertAction * _Nonnull action) {
                                                                ZegoSingleAnchorViewController *singleAnchorController = [[ZegoSingleAnchorViewController alloc] initWithNibName:@"ZegoSingleAnchorViewController" bundle:nil];
                                                                singleAnchorController.liveTitle = [self getLiveTitle];
                                                                singleAnchorController.useFrontCamera = self.cameraSwitch.on;
                                                                singleAnchorController.enableTorch = self.torchSwitch.on;
                                                                singleAnchorController.beautifyFeature = [self.beautifyPicker selectedRowInComponent:0];
                                                                singleAnchorController.filter = [self.filterPicker selectedRowInComponent:0];
                                                                
                                                                [self.previewView removeFromSuperview];
                                                                singleAnchorController.publishView = self.previewView;
                                                                self.previewView = nil;
                                                                
                                                                [self presentViewController:singleAnchorController animated:YES completion:nil];
                                                            }];
        UIAlertAction *multiAnchor = [UIAlertAction actionWithTitle:NSLocalizedString(@"连麦模式", nil)
                                                              style:UIAlertActionStyleDefault
                                                            handler:^(UIAlertAction * _Nonnull action) {
                                                                
                                                                ZegoMultiAnchorViewController *multiAnchorController = [[ZegoMultiAnchorViewController alloc] initWithNibName:@"ZegoMultiAnchorViewController" bundle:nil];
                                                                multiAnchorController.liveTitle = [self getLiveTitle];
                                                                multiAnchorController.useFrontCamera = self.cameraSwitch.on;
                                                                multiAnchorController.enableTorch = self.torchSwitch.on;
                                                                multiAnchorController.beautifyFeature = [self.beautifyPicker selectedRowInComponent:0];
                                                                multiAnchorController.filter = [self.filterPicker selectedRowInComponent:0];
                                                                
                                                                [self.previewView removeFromSuperview];
                                                                multiAnchorController.publishView = self.previewView;
                                                                self.previewView = nil;
                                                                
                                                                [self presentViewController:multiAnchorController animated:YES completion:nil];
                                                            }];
        UIAlertAction *mixStream = [UIAlertAction actionWithTitle:NSLocalizedString(@"混流模式", nil)
                                                            style:UIAlertActionStyleDefault
                                                          handler:^(UIAlertAction * _Nonnull action) {
                                                              ZegoMixStreamAnchorViewController *mixStreamAnchorController = [[ZegoMixStreamAnchorViewController alloc] initWithNibName:@"ZegoMixStreamAnchorViewController" bundle:nil];
                                                              
                                                              mixStreamAnchorController.liveTitle = [self getLiveTitle];
                                                              mixStreamAnchorController.useFrontCamera = self.cameraSwitch.on;
                                                              mixStreamAnchorController.enableTorch = self.torchSwitch.on;
                                                              mixStreamAnchorController.beautifyFeature = [self.beautifyPicker selectedRowInComponent:0];
                                                              mixStreamAnchorController.filter = [self.filterPicker selectedRowInComponent:0];
                                                              
                                                              [self.previewView removeFromSuperview];
                                                              mixStreamAnchorController.publishView = self.previewView;
                                                              self.previewView = nil;
                                                              
                                                              [self presentViewController:mixStreamAnchorController animated:YES completion:nil];
                                                          }];
        
        [alertController addAction:cancel];
//        [alertController addAction:singleAnchor];     // 屏蔽单主播直播模式
        [alertController addAction:multiAnchor];
        [alertController addAction:mixStream];
        
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

#pragma mark - Private

- (BOOL)isDeviceiOS7 {
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0) {
        return YES;
    }
    return NO;
}

- (NSString *)getLiveTitle
{
    [self.titleField resignFirstResponder];
    NSString *liveTitle = nil;
    if (self.titleField.text.length == 0)
        liveTitle = [NSString stringWithFormat:@"Hello-%@", [ZegoSetting sharedInstance].userName];
    else
    {
        if (self.titleField.text.length > MAX_TITLE_LENGTH)
            liveTitle = [self.titleField.text substringToIndex:MAX_TITLE_LENGTH];
        else
            liveTitle = self.titleField.text;
    }
    
    return liveTitle;
}

#pragma mark -- Preview

- (void)addPreviewView {
    self.previewView = [[UIView alloc] init];
    self.previewView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:self.previewView];
    [self.view sendSubviewToBack:self.previewView];
    
    [self addPreviewViewConstraints];
    
    [UIView animateWithDuration:0.1 animations:^{
        [self.view layoutIfNeeded];
    }];
}

- (void)addPreviewViewConstraints {
    [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[_previewView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(_previewView)]];
    [self.view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[_previewView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(_previewView)]];
}

- (void)startPreview {
    ZegoAVConfig *config = [ZegoSetting sharedInstance].avConfig;
    
    CGFloat height = config.videoEncodeResolution.height;
    CGFloat width = config.videoEncodeResolution.width;
    
    // 如果开播前横屏，则横置画面
    if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation)) {
        config.videoEncodeResolution = CGSizeMake(MAX(height, width), MIN(height, width));
    } else {
        config.videoEncodeResolution = CGSizeMake(MIN(height, width), MAX(height, width));
    }
    
    config.videoCaptureResolution = config.videoEncodeResolution;
    
    assert([[ZegoManager api] setAVConfig:config]);
    assert([[ZegoManager api] setFrontCam:self.cameraSwitch.on]);
    assert([[ZegoManager api] enableMic:YES]);
    assert([[ZegoManager api] enableTorch:self.torchSwitch.on]);
    
    // 美颜、滤镜
    assert([[ZegoManager api] enableBeautifying:(int)[self.beautifyPicker selectedRowInComponent:0]]);
    assert([[ZegoManager api] setFilter:(int)[self.filterPicker selectedRowInComponent:0]]);
    [[ZegoManager api] setPolishFactor:4.0];
    [[ZegoManager api] setPolishStep:4.0];
    [[ZegoManager api] setWhitenFactor:0.6];
    
    // 水印
    [[ZegoManager api] setWaterMarkImagePath:@"asset:watermark"];
    [[ZegoManager api] setPreviewWaterMarkRect:CGRectMake(10, 120, 103, 49)];
    [[ZegoManager api] setPublishWaterMarkRect:CGRectMake(10, 120, 103, 49)];
    
    // 预览模式
    [[ZegoManager api] setPreviewViewMode:ZegoVideoViewModeScaleToFill];
    [[ZegoManager api] setPreviewView:self.previewView];
    [[ZegoManager api] setDeviceEventDelegate:self];
    [[ZegoManager api] startPreview];
    
    if ([ZegoSetting sharedInstance].recordTime) {
        [[ZegoManager api] enablePreviewMirror:false];
    }
    
    if ([ZegoSetting sharedInstance].useExternalCapture) {
        [self addExternalCaptureView];
    }
}

- (void)stopPreview {
    if ([ZegoSetting sharedInstance].useExternalCapture) {
        [self removeExternalCaptureView];
    }
    
    [[ZegoManager api] setPreviewView:nil];
    [[ZegoManager api] stopPreview];
}

#pragma mark -- Record time

- (void)addExternalCaptureView
{
    if (self.videoView)
    {
        [self.videoView removeFromSuperview];
        self.videoView = nil;
    }
    
    if (self.previewTimer)
    {
        [self.previewTimer invalidate];
        self.previewTimer = nil;
    }
    
    _videoView = [[UIImageView alloc] init];
    self.videoView.translatesAutoresizingMaskIntoConstraints = YES;
    [self.previewView addSubview:self.videoView];
    self.videoView.frame = self.previewView.bounds;
    
    //timer
    self.previewTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/60 target:self selector:@selector(handlePreview) userInfo:nil repeats:YES];
}


- (void)removeExternalCaptureView
{
    [self.previewTimer invalidate];
    self.previewTimer = nil;
    
    if (self.videoView)
    {
        [self.videoView removeFromSuperview];
        self.videoView = nil;
        [self.previewView setNeedsLayout];
    }
}


#pragma mark -- Authorization

// 检查相机权限
- (BOOL)checkVideoAuthorization {
    AVAuthorizationStatus videoStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (videoStatus == AVAuthorizationStatusDenied || videoStatus == AVAuthorizationStatusRestricted) {
        return NO;
    }
    
    if (videoStatus == AVAuthorizationStatusNotDetermined) {
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
            if (granted == NO) {
                self.publishButton.enabled = NO;
            }
        }];
    }
    
    return YES;
}

// 检查麦克风权限
- (BOOL)checkAudioAuthorization {
    AVAuthorizationStatus audioStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    if (audioStatus == AVAuthorizationStatusDenied || audioStatus == AVAuthorizationStatusRestricted) {
        return NO;
    }
    
    if (audioStatus == AVAuthorizationStatusNotDetermined) {
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted) {
            if (granted == NO) {
                self.publishButton.enabled = NO;
            }
        }];
    }
    
    return YES;
}

- (void)showAlert:(NSString *)message title:(NSString *)title {
    if ([self isDeviceiOS7]) {
        // 兼容 iOS 7.0 及以下系统版本
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:title
                                                            message:message
                                                           delegate:self
                                                  cancelButtonTitle:NSLocalizedString(@"取消", nil)
                                                  otherButtonTitles:NSLocalizedString(@"设置权限", nil), nil];
        [alertView show];
    } else {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title
                                                                                 message:message
                                                                          preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:NSLocalizedString(@"取消", nil)
                                                         style:UIAlertActionStyleCancel
                                                       handler:^(UIAlertAction * _Nonnull action) {
                                                           self.publishButton.enabled = NO;
                                                       }];
        UIAlertAction *setting = [UIAlertAction actionWithTitle:NSLocalizedString(@"设置权限", nil)
                                                          style:UIAlertActionStyleDefault
                                                        handler:^(UIAlertAction * _Nonnull action) {
                                                            [self openSetting];
                                                        }];
        
        [alertController addAction:cancel];
        [alertController addAction:setting];
        alertController.preferredAction = setting;
        
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

// 打开系统设置页
- (void)openSetting {
    NSURL *settingURL = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
    if ([[UIApplication sharedApplication] canOpenURL:settingURL]) {
        [[UIApplication sharedApplication] openURL:settingURL options:nil completionHandler:nil];
    }
}

#pragma mark -- External capture

#ifdef VIDEOLIVE

- (void)handlePreview
{
    
#if TARGET_OS_SIMULATOR
    ZegoVideoCaptureFactory *demo = [[ZegoSetting sharedInstance] getVideoCaptureFactory];
#else
    VideoCaptureFactoryDemo *demo = [[ZegoSetting sharedInstance] getVideoCaptureFactory];
#endif
    
    if (demo)
    {
        UIImage *image = [demo getCaptureDevice].videoImage;
        dispatch_async(dispatch_get_main_queue(), ^{
            self.videoView.image = image;
        });
    }
}

#endif

#pragma mark - UIPickerViewDelegate

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
    if (pickerView == self.beautifyPicker) {
        return [ZegoSetting sharedInstance].beautifyList[row];
    } else {
        return [ZegoSetting sharedInstance].filterList[row];
    }
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    [self.titleField resignFirstResponder];
    
    if (pickerView == self.beautifyPicker) {
        int feature = 0;
        switch (row) {
            case 1:
                feature = ZEGO_BEAUTIFY_POLISH;
                break;
            case 2:
                feature = ZEGO_BEAUTIFY_WHITEN;
                break;
            case 3:
                feature = ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_WHITEN;
                break;
            case 4:
                feature = ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_SKINWHITEN;
                break;
                
            default:
                break;
        }
        [ZegoSetting sharedInstance].beautifyFeature = feature;
        [[ZegoManager api] enableBeautifying:feature];
    } else if (pickerView == self.filterPicker) {
        [ZegoSetting sharedInstance].filterFeature = (ZegoFilter)row;
        [[ZegoManager api] setFilter:row];
    }
}

#pragma mark - UIPickerViewDataSource

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    if (pickerView == self.beautifyPicker) {
        return [ZegoSetting sharedInstance].beautifyList.count;
    } else {
        return [ZegoSetting sharedInstance].filterList.count;
    }
}


#pragma mark - ZegoDeviceEventDelegate

- (void)zego_onDevice:(NSString *)deviceName error:(int)errorCode
{
    NSLog(@"device name: %@, error code: %d", deviceName, errorCode);
}



@end
