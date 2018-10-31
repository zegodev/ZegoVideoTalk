//
//  ZegoSingleAnchorViewController.m
//
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import "ZegoSingleAnchorViewController.h"
#import "ZegoManager.h"
#import "ZegoSetting.h"
#import "ZegoAnchorOptionViewController.h"
#import "ZegoLiveToolViewController.h"


@interface ZegoSingleAnchorViewController () <ZegoRoomDelegate, ZegoLivePublisherDelegate, ZegoIMDelegate, ZegoLiveToolViewControllerDelegate>

@property (weak, nonatomic) IBOutlet UIView *playViewContainer;                 // 容器 view，用于容纳直播画面 view
@property (weak, nonatomic) IBOutlet UIView *toolView;                          // 工具 view，用于容纳 toolViewController

@property (nonatomic, weak) ZegoLiveToolViewController *toolViewController;     // 内嵌的直播设置 viewcontroller

@property (nonatomic, weak) UIButton *stopPublishButton;                        // 停止直播
@property (nonatomic, weak) UIButton *mutedButton;                              // 静音
@property (nonatomic, weak) UIButton *sharedButton;                             // 分享

@property (nonatomic, copy) NSString *streamID;                                 // 正在直播的单流流 ID

@property (nonatomic, strong) NSMutableDictionary *viewContainersDict;

@property (nonatomic, assign) BOOL isPublishing;                                // 正在直播标志位

@property (nonatomic, strong) UIColor *defaultButtonColor;
@property (nonatomic, strong) UIColor *disableButtonColor;

@property (nonatomic, copy) NSString *sharedHls;
@property (nonatomic, copy) NSString *sharedRtmp;
@property (nonatomic, copy) NSString *roomID;

@property (nonatomic, assign) UIInterfaceOrientation orientation;

@end

@implementation ZegoSingleAnchorViewController

#pragma mark - Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupLiveKit];
    [self loginChatRoom];
    
    _viewContainersDict = [[NSMutableDictionary alloc] initWithCapacity:self.maxStreamCount];
    
    // 配置嵌套在其中的 ZegoLiveToolViewController
    ZegoLiveToolViewController *toolController = [[ZegoLiveToolViewController alloc] initWithNibName:@"ZegoLiveToolViewController" bundle:nil];
    [self displayToolController:toolController];
    
    self.stopPublishButton = self.toolViewController.stopPublishButton;
    self.mutedButton = self.toolViewController.mutedButton;
    self.sharedButton = self.toolViewController.shareButton;
    
    self.stopPublishButton.enabled = NO;
    [self setSharedButtonEnable:NO];
    
    self.mutedButton.enabled = NO;
    self.defaultButtonColor = [self.mutedButton titleColorForState:UIControlStateNormal];
    self.disableButtonColor = [self.mutedButton titleColorForState:UIControlStateDisabled];
    
    self.orientation = [UIApplication sharedApplication].statusBarOrientation;
    
    if (self.publishView)
    {
        [self updatePublishView:self.publishView];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Private

- (BOOL)shouldAutorotate
{
    return NO;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    if (self.orientation == UIInterfaceOrientationPortrait)
        return UIInterfaceOrientationMaskPortrait;
    else if (self.orientation == UIInterfaceOrientationLandscapeLeft)
        return UIInterfaceOrientationMaskLandscapeLeft;
    else if (self.orientation == UIInterfaceOrientationLandscapeRight)
        return UIInterfaceOrientationMaskLandscapeRight;
    
    return UIInterfaceOrientationMaskPortrait;
}

- (void)displayToolController:(ZegoLiveToolViewController *)toolController {
    [self addChildViewController:toolController];
    [self.toolView addSubview:toolController.view];
    [toolController didMoveToParentViewController:self];
    self.toolViewController = toolController;
    self.toolViewController.delegate = self;
    self.toolViewController.isAudience = NO;
    self.toolViewController.view.frame = self.toolView.frame;
}

- (void)setupLiveKit
{
    [[ZegoManager api] setRoomDelegate:self];
    [[ZegoManager api] setPublisherDelegate:self];
    [[ZegoManager api] setIMDelegate:self];
}

- (bool)doPublish
{
    //登录成功后配置直播参数，开始直播 创建publishView
    if (self.publishView.superview == nil)
        self.publishView = nil;
    
    if (self.publishView == nil)
    {
        self.publishView = [self createPublishView];
        if (self.publishView)
        {
            [self setAnchorConfig:self.publishView];
            [[ZegoManager api] startPreview];
        }
    }
    
    self.viewContainersDict[self.streamID] = self.publishView;
    
    bool b = [[ZegoManager api] startPublishing:self.streamID title:self.liveTitle flag:ZEGOAPI_SINGLE_ANCHOR];
    if (b)
    {
        [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"开始直播，流ID:%@", nil), self.streamID]];
    }
    return b;
}

- (void)stopPublishing
{
    [[ZegoManager api] stopPreview];
    [[ZegoManager api] setPreviewView:nil];
    [[ZegoManager api] stopPublishing];
    
    [self removeStreamViewContainer:self.streamID];
    self.publishView = nil;
    
    self.isPublishing = NO;
}

- (void)loginChatRoom
{
    self.roomID = [ZegoSetting getMyRoomID:SinglePublisherRoom];
    self.streamID = [ZegoSetting getPublishStreamID];

    [[ZegoManager api] loginRoom:self.roomID roomName:self.liveTitle role:ZEGO_ANCHOR  withCompletionBlock:^(int errorCode, NSArray<ZegoStream *> *streamList) {
        
        NSLog(@"%s, error: %d", __func__, errorCode);
        if (errorCode == 0)
        {
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"登录房间成功. roomID: %@", nil), self.roomID];
            [self addLogString:logString];
            [self doPublish];
        }
        else
        {
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"登录房间失败. error: %d", nil), errorCode];
            [self addLogString:logString];
        }
        
    }];
    
    [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"开始登录房间", nil)]];
}

- (BOOL)updatePublishView:(UIView *)publishView
{
    publishView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.playViewContainer addSubview:publishView];
    
    BOOL bResult = [self setContainerConstraints:publishView containerView:self.playViewContainer viewIndex:self.playViewContainer.subviews.count - 1];
    if (bResult == NO)
    {
        [publishView removeFromSuperview];
        return NO;
    }
    
    [self.playViewContainer bringSubviewToFront:publishView];
    return YES;
}

- (UIView *)createPublishView
{
    UIView *publishView = [[UIView alloc] init];
    publishView.translatesAutoresizingMaskIntoConstraints = NO;
    
    BOOL result = [self updatePublishView:publishView];
    if (result == NO)
        return nil;
    
    return publishView;
}

- (void)removeStreamViewContainer:(NSString *)streamID
{
    UIView *view = self.viewContainersDict[streamID];
    if (view == nil)
        return;
    
    [self updateContainerConstraintsForRemove:view containerView:self.playViewContainer];
    
    [self.viewContainersDict removeObjectForKey:streamID];
}

- (void)closeAllStream
{
    [self stopPublishing];
}

- (void)setSharedButtonEnable:(BOOL)enable {
    if (enable) {
        self.sharedButton.enabled = YES;
        [self.sharedButton setImage:[UIImage imageNamed:@"share_enable"] forState:UIControlStateNormal];
    } else {
        self.sharedButton.enabled = NO;
        [self.sharedButton setImage:[UIImage imageNamed:@"share_disable"] forState:UIControlStateNormal];
    }
}

#pragma mark - ZegoRoomDelegate

- (void)onDisconnect:(int)errorCode roomID:(NSString *)roomID
{
    NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"连接失败, error: %d", nil), errorCode];
    [self addLogString:logString];
}

- (void)onKickOut:(int)reason roomID:(NSString *)roomID
{
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"" message:NSLocalizedString(@"被踢出房间", nil) delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
    [alertView show];
    
    [self onCloseButton:nil];
}

#pragma mark - ZegoLivePublisherDelegate

- (void)onPublishStateUpdate:(int)stateCode streamID:(NSString *)streamID streamInfo:(NSDictionary *)info
{
    NSLog(@"%s, stream: %@, state: %d", __func__, streamID, stateCode);
    
    NSString *logString = nil;
    
    if (stateCode == 0)
    {
        self.isPublishing = YES;
        
        [self.stopPublishButton setTitle:NSLocalizedString(@"停止直播", nil) forState:UIControlStateNormal];
        
        self.sharedHls = [info[kZegoHlsUrlListKey] firstObject];
        self.sharedRtmp = [info[kZegoRtmpUrlListKey] firstObject];
        
        [self addLogString:[NSString stringWithFormat:@"Hls %@", self.sharedHls]];
        [self addLogString:[NSString stringWithFormat:@"Rtmp %@", self.sharedRtmp]];
        
        logString = [NSString stringWithFormat:NSLocalizedString(@"发布直播成功,流ID:%@", nil), streamID];
        
        if (self.sharedHls.length > 0 && self.sharedRtmp.length > 0)
        {
            [self setSharedButtonEnable:YES];
            
            NSDictionary *dict = @{kHlsKey: self.sharedHls, kRtmpKey: self.sharedRtmp};
            NSString *jsonString = [self encodeDictionaryToJSON:dict];
            if (jsonString)
                [[ZegoManager api] updateStreamExtraInfo:jsonString];
        }
        else
        {
            [self setSharedButtonEnable:NO];
        }
    }
    else
    {
        self.isPublishing = NO;
        [self removeStreamViewContainer:streamID];
        self.publishView = nil;
        [self setSharedButtonEnable:NO];
        
        [self.stopPublishButton setTitle:NSLocalizedString(@"开始直播", nil) forState:UIControlStateNormal];
        
        logString = [NSString stringWithFormat:NSLocalizedString(@"直播结束,流ID：%@, error:%d", nil), streamID, stateCode];
    }
    
    [self addLogString:logString];
    
    self.stopPublishButton.enabled = YES;
}

- (void)onPublishQualityUpdate:(NSString *)streamID quality:(ZegoApiPublishQuality)quality
{
    NSString *detail = [self addStaticsInfo:YES stream:streamID fps:quality.fps kbs:quality.kbps rtt:quality.rtt pktLostRate:quality.pktLostRate];
    
    UIView *view = self.viewContainersDict[streamID];
    if (view)
        [self updateQuality:quality.quality detail:detail onView:view];
    
}

- (void)onAuxCallback:(void *)pData dataLen:(int *)pDataLen sampleRate:(int *)pSampleRate channelCount:(int *)pChannelCount
{
    [self auxCallback:pData dataLen:pDataLen sampleRate:pSampleRate channelCount:pChannelCount];
}

#pragma mark - ZegoIMDelegate

- (void)onRecvRoomMessage:(NSString *)roomId messageList:(NSArray<ZegoRoomMessage *> *)messageList
{
    [self.toolViewController updateLayout:messageList];
}

#pragma mark - ZegoLiveToolViewControllerDelegate

- (void)onCloseButton:(id)sender
{
    [self closeAllStream];
    [[ZegoManager api] logoutRoom];
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)onMutedButton:(id)sender
{
    if (self.enableSpeaker)
    {
        self.enableSpeaker = NO;
        [self.mutedButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
    }
    else
    {
        self.enableSpeaker = YES;
        [self.mutedButton setTitleColor:self.defaultButtonColor forState:UIControlStateNormal];
    }
}

- (void)onOptionButton:(id)sender
{
    [self showPublishOption];
}

- (void)onStopPublishButton:(id)sender
{
    if (self.isPublishing)
    {
        [self stopPublishing];
        
        // * update button
        [self.stopPublishButton setTitle:NSLocalizedString(@"开始直播", nil) forState:UIControlStateNormal];
        self.stopPublishButton.enabled = YES;
    }
    else if ([[self.stopPublishButton currentTitle] isEqualToString:NSLocalizedString(@"开始直播", nil)])
    {
        [self doPublish];
        self.stopPublishButton.enabled = NO;
    }
}

- (void)onLogButton:(id)sender
{
    [self showLogViewController];
}

- (void)onShareButton:(id)sender
{
    if (self.sharedHls.length == 0) {
        return;
    }
    
    [self shareToQQ:self.sharedHls rtmp:self.sharedRtmp bizToken:nil bizID:self.roomID streamID:self.streamID];
}

- (void)onSendComment:(NSString *)comment
{
    bool ret = [[ZegoManager api] sendRoomMessage:comment type:ZEGO_TEXT category:ZEGO_CHAT priority:ZEGO_DEFAULT completion:nil];
    if (ret)
    {
        ZegoRoomMessage *roomMessage = [ZegoRoomMessage new];
        roomMessage.fromUserId = [ZegoSetting sharedInstance].userID;
        roomMessage.fromUserName = [ZegoSetting sharedInstance].userName;
        roomMessage.content = comment;
        roomMessage.type = ZEGO_TEXT;
        roomMessage.category = ZEGO_CHAT;
        roomMessage.priority = ZEGO_DEFAULT;
        
        [self.toolViewController updateLayout:@[roomMessage]];
    }
}

- (void)onSendLike
{
    //    [[ZegoManager api] likeAnchor:1 count:10];
    NSDictionary *likeDict = @{@"likeType": @(1), @"likeCount": @(10)};
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:likeDict options:0 error:nil];
    NSString *content = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    
    bool ret = [[ZegoManager api] sendRoomMessage:content type:ZEGO_TEXT category:ZEGO_LIKE priority:ZEGO_DEFAULT completion:nil];
    if (ret)
    {
        ZegoRoomMessage *roomMessage = [ZegoRoomMessage new];
        roomMessage.fromUserId = [ZegoSetting sharedInstance].userID;
        roomMessage.fromUserName = [ZegoSetting sharedInstance].userName;
        roomMessage.content = NSLocalizedString(@"点赞了主播", nil);
        roomMessage.type = ZEGO_TEXT;
        roomMessage.category = ZEGO_CHAT;
        roomMessage.priority = ZEGO_DEFAULT;
        
        [self.toolViewController updateLayout:@[roomMessage]];
    }
}

@end
