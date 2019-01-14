//
//  ZegoTalkViewController.m
//  VideoTalk
//
//  Created by summery on 23/10/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "ZegoTalkViewController.h"
#import "ZegoTalkToolViewController.h"
#import "ZegoManager.h"
#import "ZegoSetting.h"

@interface ZegoTalkViewController () <ZegoRoomDelegate, ZegoLivePublisherDelegate, ZegoLivePlayerDelegate, ZegoTalkToolViewControllerDelegate>

@property (weak, nonatomic) IBOutlet UIView *toolView;
@property (weak, nonatomic) IBOutlet UIView *playViewContainer;

@property (nonatomic, strong) ZegoTalkToolViewController *toolViewController;

@property (nonatomic, weak) UIButton *cameraButton;
@property (nonatomic, weak) UIButton *switchCameraButton;
@property (nonatomic, weak) UIButton *micButton;
@property (nonatomic, weak) UIButton *muteButton;
@property (nonatomic, weak) UIButton *logButton;
@property (nonatomic, weak) UIButton *closeButton;

@property (nonatomic, strong) NSMutableArray<ZegoStream *> *streamList;

@property (nonatomic, assign) BOOL isLoginSucceed;      // 是否登录成功
@property (nonatomic, assign) BOOL isPublishing;        // 是否正在推流

@property (nonatomic, strong) NSMutableDictionary *viewContainersDict;
@property (nonatomic, strong) NSMutableDictionary *streamID2SizeDict;
@property (nonatomic, strong) NSMutableDictionary *videoSizeDict;

@property (nonatomic, copy) NSString *publishTitle;
@property (nonatomic, copy) NSString *publishStreamID;  // 推流 ID

@property (nonatomic, strong) UIColor *defaultButtonColor;
@property (nonatomic, strong) UIColor *disableButtonColor;


@end

@implementation ZegoTalkViewController

#pragma mark - Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    //开启监听设备旋转
    if (![UIDevice currentDevice].generatesDeviceOrientationNotifications) {
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    }
    
    //注意监听的是UIApplicationDidChangeStatusBarOrientationNotification而不是Device
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(handleOrientationChange:)
                                                name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
    
    ZegoTalkToolViewController *toolController = [[ZegoTalkToolViewController alloc] initWithNibName:@"ZegoTalkToolViewController" bundle:nil];
    [self displayToolViewController:toolController];

    self.cameraButton = self.toolViewController.cameraButton;
    self.micButton = self.toolViewController.micButton;
    self.muteButton = self.toolViewController.muteButton;
    self.logButton = self.toolViewController.logButton;
    self.closeButton = self.toolViewController.closeButton;
    self.switchCameraButton = self.toolViewController.switchCameraButton;
    
    self.defaultButtonColor = [UIColor colorWithRed:76/255.0 green:76/255.0 blue:76/255.0 alpha:0.3];
    self.disableButtonColor = [UIColor colorWithRed:13/255.0 green:112/255.0 blue:255/255.0 alpha:0.9];
    
    self.streamList = [[NSMutableArray alloc] initWithCapacity:self.maxStreamCount];
    self.viewContainersDict = [[NSMutableDictionary alloc] initWithCapacity:self.maxStreamCount];
    self.videoSizeDict = [[NSMutableDictionary alloc] initWithCapacity:self.maxStreamCount];
    self.streamID2SizeDict = [[NSMutableDictionary alloc] initWithCapacity:self.maxStreamCount];
    
    [self setupLiveKit];
    [self loginRoom];
}

- (void)viewDidLayoutSubviews {
    UIImage *backgroundImage = [[ZegoSetting sharedInstance] getBackgroundImage:self.view.bounds.size withText:NSLocalizedString(@"加载中...", nil)];
    [self setBackgroundImage:backgroundImage playerView:self.playViewContainer];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Private

- (void)displayToolViewController:(ZegoTalkToolViewController *)controller {
    [self addChildViewController:controller];
    [self.toolView addSubview:controller.view];
    [controller didMoveToParentViewController:self];
    
    self.toolViewController = controller;
    self.toolViewController.delegate = self;
    self.toolViewController.view.frame = self.toolView.frame;
}

- (void)setupLiveKit {
    [[ZegoManager api] setRoomDelegate:self];
    [[ZegoManager api] setPublisherDelegate:self];
    [[ZegoManager api] setPlayerDelegate:self];
}

- (void)loginRoom {
    if (self.roomID.length) {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"开始登录房间", nil)];
        [self addLogString:logString];
        
        bool logining = [[ZegoManager api] loginRoom:self.roomID role:ZEGO_AUDIENCE withCompletionBlock:^(int errorCode, NSArray<ZegoStream *> *streamList) {
            NSLog(@"%s, error: %d", __func__, errorCode);
            if (errorCode == 0) {
                NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"登录房间成功. roomID: %@", nil), self.roomID];
                [self addLogString:logString];
                
                self.isLoginSucceed = YES;
                
                if (streamList.count) {
                    [self onStreamUpdateForAdd:streamList]; // 登录成功即拉流
                }
            } else {
                NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"登录房间失败. error: %d", nil), errorCode];
                [self addLogString:logString];
                
                self.isLoginSucceed = NO;
            }
        }];
        
        if (logining) {
            // 调用 loginRoom 接口后立即推流，加快推流速度
            [self doPublish];
        } else {
            [self addLogString:[NSString stringWithFormat:@"登录房间 %@ 失败", self.roomID]];
        }
    }
}

- (BOOL)isStreamIDExist:(NSString *)streamID
{
    if ([self.publishStreamID isEqualToString:streamID])
        return YES;
    
    for (ZegoStream *info in self.streamList)
    {
        if ([info.streamID isEqualToString:streamID])
            return YES;
    }
    
    return NO;
}

- (void)setBackgroundImage:(UIImage *)image playerView:(UIView *)playerView
{
    playerView.backgroundColor = [UIColor colorWithPatternImage:image];
}

#pragma mark -- Add play stream

- (void)onStreamUpdateForAdd:(NSArray<ZegoStream *> *)streamList
{
    for (ZegoStream *stream in streamList)
    {
        NSString *streamID = stream.streamID;
        if (streamID.length == 0)
            continue;
        
        if ([self isStreamIDExist:streamID])
            continue;
        
        if (self.viewContainersDict.count >= self.maxStreamCount)
            return;
        
        [self.streamList addObject:stream];
        [self addStreamViewContainer:streamID];
        
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"新增一条流, 流ID:%@", nil), streamID];
        [self addLogString:logString];
    }
}

- (void)addStreamViewContainer:(NSString *)streamID
{
    if (streamID.length == 0)
        return;
    
    if (self.viewContainersDict[streamID] != nil)
        return;
    
    UIView *bigView = [[UIView alloc] init];
    bigView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.playViewContainer addSubview:bigView];
    
    BOOL bResult = [self setContainerConstraints:bigView containerView:self.playViewContainer viewIndex:self.viewContainersDict.count];
    if (bResult == NO) {
        [bigView removeFromSuperview];
        return;
    }
    
    UIImage *backgroundImage = [[ZegoSetting sharedInstance] getBackgroundImage:self.view.bounds.size withText:NSLocalizedString(@"加载中", nil)];
    [self setBackgroundImage:backgroundImage playerView:bigView];
    
    self.viewContainersDict[streamID] = bigView;
    bool ret = [[ZegoManager api] startPlayingStream:streamID inView:bigView];
    [[ZegoManager api] setViewMode:ZegoVideoViewModeScaleAspectFill ofStream:streamID];
    [[ZegoManager api] activateVideoPlayStream:streamID active:YES videoLayer:VideoStreamLayer_Auto];


    assert(ret);
}

#pragma mark -- Delete play stream

- (void)onStreamUpdateForDelete:(NSArray<ZegoStream *> *)streamList
{
    for (ZegoStream *stream in streamList)
    {
        NSString *streamID = stream.streamID;
        if (streamID.length == 0)
            continue;
        
        if (![self isStreamIDExist:streamID])
            continue;
        
        [[ZegoManager api] stopPlayingStream:streamID];
        
        [self removeStreamViewContainer:streamID];
        [self removeStreamInfo:streamID];
        
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"删除一条流, 流ID:%@", nil), streamID];
        [self addLogString:logString];
    
    }
}

- (void)removeStreamViewContainer:(NSString *)streamID
{
    if (streamID.length == 0)
        return;
    
    UIView *view = self.viewContainersDict[streamID];
    if (view == nil)
        return;
    
    [self updateContainerConstraintsForRemove:view containerView:self.playViewContainer];
    
    [self.viewContainersDict removeObjectForKey:streamID];
}

- (void)removeStreamInfo:(NSString *)streamID
{
    NSInteger index = NSNotFound;
    for (ZegoStream *info in self.streamList)
    {
        if ([info.streamID isEqualToString:streamID])
        {
            index = [self.streamList indexOfObject:info];
            break;
        }
    }
    
    if (index != NSNotFound)
        [self.streamList removeObjectAtIndex:index];
}

#pragma mark -- Publish stream

- (void)doPublish {
#ifdef VIDEOTALK
    self.useFrontCamera = YES;
#endif
    
    [self createPublishStream];
}

- (void)createPublishStream
{
    self.publishTitle = [NSString stringWithFormat:@"Hello-%@", [ZegoSetting sharedInstance].userName];
    NSString *streamID = [ZegoSetting getPublishStreamID];
    
    NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"创建流成功, streamID:%@", nil), streamID];
    [self addLogString:logString];
    
    //创建发布view
    UIView *publishView = [self createPublishView:streamID];
    if (publishView)
    {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"开始发布直播", nil)];
        [self addLogString:logString];
        
        [self setAnchorConfig:publishView];
    
        logString = [NSString stringWithFormat:NSLocalizedString(@"setVideoCodecId: %ld, channel: MAIN", nil), [ZegoSetting sharedInstance].videoCodecType];
        [self addLogString:logString];
        
        [[ZegoManager api] setVideoCodecId:(ZegoVideoCodecAvc)([ZegoSetting sharedInstance].videoCodecType) ofChannel:ZEGOAPI_CHN_MAIN];
        [[ZegoManager api] startPublishing:streamID title:self.publishTitle flag:ZEGO_JOIN_PUBLISH];
    }
}

- (UIView *)createPublishView:(NSString *)streamID
{
    if (streamID.length == 0)
        return nil;
    
    UIView *publishView = [[UIView alloc] init];
    publishView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.playViewContainer addSubview:publishView];
    
    BOOL bResult = [self setContainerConstraints:publishView containerView:self.playViewContainer viewIndex:self.viewContainersDict.count];
    if (bResult == NO)
    {
        [publishView removeFromSuperview];
        return nil;
    }
    
    self.viewContainersDict[streamID] = publishView;
    [self.playViewContainer bringSubviewToFront:publishView];
    
    return publishView;
}

- (void)clearAllStream
{
    for (ZegoStream *info in self.streamList)
    {
        [[ZegoManager api] stopPlayingStream:info.streamID];
        UIView *playView = self.viewContainersDict[info.streamID];
        if (playView)
        {
            [self updateContainerConstraintsForRemove:playView containerView:self.playViewContainer];
            [self.viewContainersDict removeObjectForKey:info.streamID];
        }
    }
    
    [self stopPublishing];
    
    [self.viewContainersDict removeAllObjects];
}

- (void)stopPublishing
{
    if (self.isPublishing)
    {
        [[ZegoManager api] stopPreview];
        [[ZegoManager api] setPreviewView:nil];
        [[ZegoManager api] stopPublishing];
        [self removeStreamViewContainer:self.publishStreamID];
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

- (void)onStreamUpdated:(int)type streams:(NSArray<ZegoStream *> *)streamList roomID:(NSString *)roomID
{
    if (type == ZEGO_STREAM_ADD)
        [self onStreamUpdateForAdd:streamList];
    else if (type == ZEGO_STREAM_DELETE)
        [self onStreamUpdateForDelete:streamList];
}

- (void)onStreamExtraInfoUpdated:(NSArray<ZegoStream *> *)streamList roomID:(NSString *)roomID
{
    for (ZegoStream *stream in streamList)
    {
        for (ZegoStream *stream1 in self.streamList)
        {
            if (stream.streamID == stream1.streamID)
            {
                stream1.extraInfo = stream.extraInfo;
                break;
            }
        }
    }
}


#pragma mark - ZegoLivePlayerDelegate

- (void)onPlayStateUpdate:(int)stateCode streamID:(NSString *)streamID
{
    NSLog(@"%s, streamID:%@", __func__, streamID);
    
    if (stateCode == 0)
    {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"播放流成功, 流ID:%@", nil), streamID];
        [self addLogString:logString];
    }
    else
    {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"播放流失败, 流ID:%@,  error:%d", nil), streamID, stateCode];
        [self addLogString:logString];
    }
}

- (void)onVideoSizeChangedTo:(CGSize)size ofStream:(NSString *)streamID
{
    NSLog(@"%s, streamID %@", __func__, streamID);
    
    NSLog(@" onVideoSizeChangedTo, height %f, width %f", size.height, size.width);
    
    NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"第一帧画面, 流ID:%@", nil), streamID];
    [self addLogString:logString];
    
    UIView *view = self.viewContainersDict[streamID];
    if (view)
        [self setBackgroundImage:nil playerView:view];
    
    if ([self.publishStreamID isEqualToString:streamID])
        return;
    
    if ([self isStreamIDExist:streamID] && view)
    {
        if (size.width > size.height && view.frame.size.width < view.frame.size.height)
        {
            [[ZegoManager api] setViewMode:ZegoVideoViewModeScaleAspectFit ofStream:streamID];
            
            self.videoSizeDict[streamID] = @(NO);
        }
        
        self.streamID2SizeDict[streamID] = [NSValue valueWithCGSize:size];
    }
}

- (void)onPlayQualityUpate:(NSString *)streamID quality:(ZegoApiPlayQuality)quality
{
    NSString *detail = [self addStaticsInfo:NO stream:streamID fps:quality.fps kbs:quality.kbps rtt:quality.rtt pktLostRate:quality.pktLostRate];
    
    UIView *view = self.viewContainersDict[streamID];
    if (view)
        [self updateQuality:quality.quality detail:detail onView:view];
}

#pragma mark - ZegoLivePublisherDelegate

- (void)onPublishStateUpdate:(int)stateCode streamID:(NSString *)streamID streamInfo:(NSDictionary *)info
{
    NSLog(@"%s, stream: %@", __func__, streamID);
    
    if (stateCode == 0)
    {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"发布直播成功,流ID:%@", nil), streamID];
        [self addLogString:logString];
        
        //记录当前的发布信息
        self.isPublishing = YES;
        self.publishStreamID = streamID;
    }
    else
    {
        if (stateCode != 1)
        {
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"发布直播失败, 流ID:%@, err:%d", nil), streamID, stateCode];
            [self addLogString:logString];
        }
        else
        {
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"发布直播结束, 流ID:%@", nil), streamID];
            [self addLogString:logString];
        }
        
        NSLog(@"%s, stream: %@, err: %u", __func__, streamID, stateCode);
        self.isPublishing = NO;
        //删除publish的view
        [self removeStreamViewContainer:streamID];
        
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"提示"
                                                                                 message:[NSString stringWithFormat:@"推流失败(错误码：%d)\n请退出房间后重试", stateCode]
                                                                          preferredStyle:UIAlertControllerStyleAlert];
        UIAlertAction *confirm = [UIAlertAction actionWithTitle:@"确定"
                                                          style:UIAlertActionStyleDefault
                                                        handler:^(UIAlertAction * _Nonnull action) {
                                                            [self onCloseButton:nil];
                                                        }];
        
        [alertController addAction:confirm];
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

- (void)onAuxCallback:(void *)pData dataLen:(int *)pDataLen sampleRate:(int *)pSampleRate channelCount:(int *)pChannelCount
{
    [self auxCallback:pData dataLen:pDataLen sampleRate:pSampleRate channelCount:pChannelCount];
}

- (void)onPublishQualityUpdate:(NSString *)streamID quality:(ZegoApiPublishQuality)quality
{
    NSString *detail = [self addStaticsInfo:YES stream:streamID fps:quality.fps kbs:quality.kbps rtt:quality.rtt pktLostRate:quality.pktLostRate];
    
    UIView *view = self.viewContainersDict[streamID];
    if (view) {
        [self updateQuality:quality.quality detail:detail onView:view];
    }
}

#pragma mark - ZegoTalkToolViewControllerDelegate

- (void)onCloseButton:(id)sender
{
    [self setBackgroundImage:nil playerView:self.playViewContainer];
    
    [self clearAllStream];
    
    if (self.isLoginSucceed) {
        [[ZegoManager api] logoutRoom];
    }

    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)onLogButton:(id)sender {
    [self showLogViewController];
}

- (void)onMuteButton:(id)sender {
    if (self.enableSpeaker) {
        self.enableSpeaker = NO;
        [self.muteButton setImage:[UIImage imageNamed:@"speaker_disable"] forState:UIControlStateNormal];
        [self.muteButton setBackgroundColor:self.disableButtonColor];
    } else {
        self.enableSpeaker = YES;
        [self.muteButton setImage:[UIImage imageNamed:@"speaker"] forState:UIControlStateNormal];
        [self.muteButton setBackgroundColor:self.defaultButtonColor];
    }
}

- (void)onMicButton:(id)sender {
    if (self.enableMicrophone) {
        self.enableMicrophone = NO;
        [self.micButton setImage:[UIImage imageNamed:@"mic_disable"] forState:UIControlStateNormal];
        [self.micButton setBackgroundColor:self.disableButtonColor];
    } else {
        self.enableMicrophone = YES;
        [self.micButton setImage:[UIImage imageNamed:@"mic"] forState:UIControlStateNormal];
        [self.micButton setBackgroundColor:self.defaultButtonColor];
    }
}

- (void)onCameraButton:(id)sender {
    if (self.enableCamera) {
        self.enableCamera = NO;
        [self.cameraButton setImage:[UIImage imageNamed:@"camera_disable"] forState:UIControlStateNormal];
        [self.cameraButton setBackgroundColor:self.disableButtonColor];
    } else {
        self.enableCamera = YES;
        [self.cameraButton setImage:[UIImage imageNamed:@"camera"] forState:UIControlStateNormal];
        [self.cameraButton setBackgroundColor:self.defaultButtonColor];
    }
}

- (void)onSwitchCameraButton:(id)sender {
    if (self.useFrontCamera) {
        self.useFrontCamera = NO;
    } else {
        self.useFrontCamera = YES;
    }
}

- (void)onTapViewPoint:(CGPoint)point
{
    CGPoint containerPoint = [self.view.window convertPoint:point toView:self.playViewContainer];
    
    for (UIView *view in self.playViewContainer.subviews)
    {
        if (CGRectContainsPoint(view.frame, containerPoint) &&
            !CGSizeEqualToSize(self.playViewContainer.bounds.size, view.frame.size))
        {
            [self onTapView:view];
            break;
        }
    }
}

- (void)onTapView:(UIView *)view
{
    if (view == nil)
        return;
    
    [self updateContainerConstraintsForTap:view containerView:self.playViewContainer];
    
    UIView *firstView = [self getFirstViewInContainer:self.playViewContainer];
    NSString *streamID = [self getStreamIDFromView:firstView];
    if (streamID == nil) {
        return;
    }
}

- (NSString *)getStreamIDFromView:(UIView *)view
{
    for (NSString *streamID in self.viewContainersDict)
    {
        if (self.viewContainersDict[streamID] == view)
            return streamID;
    }
    
    return nil;
}


#pragma mark - Orientation
//设备方向改变的处理
- (void)handleOrientationChange:(NSNotification *)notification{
    [[ZegoManager api] setAppOrientation:[UIApplication sharedApplication].statusBarOrientation];
}


@end
