//
//  ZegoAnchorToolViewController.m
//  
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import "ZegoLiveToolViewController.h"
#import "ZegoMessageTableViewCell.h"
#import "ZegoLikeView.h"

@interface ZegoLiveToolViewController ()

// 评论
@property (weak, nonatomic) IBOutlet UIButton *commentButton;
@property (weak, nonatomic) IBOutlet UIView *commentView;
@property (weak, nonatomic) IBOutlet UITextField *commentTextField;
@property (nonatomic, weak) IBOutlet UIButton *sendButton;

// 点赞
@property (weak, nonatomic) IBOutlet UIButton *likeButton;
@property (weak, nonatomic) IBOutlet ZegoLikeView *likeShowView;    // 点赞后展示赞效果

// 消息展示
@property (nonatomic, weak) IBOutlet UITableView *messageView;

// 主播设置选项
@property (nonatomic, weak) IBOutlet UIView *anchorView;

// 观众设置选项
@property (nonatomic, weak) IBOutlet UIView *audienceView;

// 关闭
@property (weak, nonatomic) IBOutlet UIButton *closeButton;

// 约束
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *commentViewBottomToSuperviewBottom;
//@property (nonatomic, weak) IBOutlet NSLayoutConstraint *commentViewHeight;
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *audienceViewTopToSuperviewTop;
//@property (nonatomic, weak) IBOutlet NSLayoutConstraint *topViewHeight;
//@property (nonatomic, weak) IBOutlet NSLayoutConstraint *topViewBottomToSuperviewBottom;

@property (nonatomic, strong) NSMutableArray<YYTextLayout*> *liveMessageList;
@property (nonatomic, assign) NSTimeInterval currentTime;
@property (nonatomic, strong) NSTimer *timer;
@property (nonatomic, assign) NSUInteger likeCount;
@property (nonatomic, strong) NSTimer *recordTimer;
@property (nonatomic, strong) NSDateFormatter *timeFormatter;

@end

@implementation ZegoLiveToolViewController

#pragma mark - Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.view.backgroundColor = [UIColor clearColor];
    
//    self.topViewHeight.constant = CGRectGetHeight(self.view.frame) - self.topViewBottomToSuperviewBottom.constant - self.commentViewHeight.constant;
    
    self.liveMessageList = [NSMutableArray array];
    self.isAudience = NO;
    self.renderLabel.hidden = YES;
    
    self.sendButton.layer.cornerRadius = 4;
    self.commentButton.layer.cornerRadius = self.commentButton.frame.size.width / 2;
    self.likeButton.layer.cornerRadius = self.likeButton.frame.size.width / 2;
    self.shareButton.layer.cornerRadius = self.shareButton.frame.size.width / 2;
    self.closeButton.layer.cornerRadius = self.closeButton.frame.size.width / 2;
    
    self.messageView.allowsSelection = NO;
    self.messageView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.messageView.backgroundColor = [UIColor clearColor];
    self.messageView.tableFooterView = [[UIView alloc] init];
    [self.messageView registerClass:[ZegoMessageTableViewCell class] forCellReuseIdentifier:@"liveMessageIdentifier"];
    
//    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillChangeFrame:) name:UIKeyboardWillChangeFrameNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
    
    UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapView:)];
    [self.view addGestureRecognizer:tapGestureRecognizer];

    self.timer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateTimeLabel) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:self.timer forMode:NSRunLoopCommonModes];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    [self.commentTextField resignFirstResponder];
    
    [self.timer invalidate];
    self.timer = nil;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

#pragma mark - Event response

#pragma mark -- Anchor and audience both

- (IBAction)onLog:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onLogButton:)])
        [self.delegate onLogButton:sender];
}

- (IBAction)onMute:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onMutedButton:)])
        [self.delegate onMutedButton:sender];
}

- (IBAction)onOption:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onOptionButton:)])
        [self.delegate onOptionButton:sender];
}

- (IBAction)onLike:(id)sender {
    //点赞
    [self.likeShowView doLikeAnimation];
    NSTimeInterval currentTime = [[NSDate date] timeIntervalSince1970];
    if (self.currentTime == 0 || currentTime - self.currentTime < 60)
    {
        self.currentTime = [[NSDate date] timeIntervalSince1970];
        if ([self.delegate respondsToSelector:@selector(onSendLike)])
            [self.delegate onSendLike];
    }
}

- (IBAction)onComment:(id)sender
{
    [self.commentTextField becomeFirstResponder];
}

- (IBAction)onSendMessage:(id)sender
{
    if (self.commentTextField.text.length != 0)
    {
        if ([self.delegate respondsToSelector:@selector(onSendComment:)])
            [self.delegate onSendComment:self.commentTextField.text];
        
        self.commentTextField.text = @"";
    }
}

- (IBAction)onClose:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onCloseButton:)])
        [self.delegate onCloseButton:sender];
}

- (IBAction)onShare:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onShareButton:)])
        [self.delegate onShareButton:sender];
}

#pragma mark -- Anchor only

- (IBAction)onStopPublish:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onStopPublishButton:)])
        [self.delegate onStopPublishButton:sender];
}

#pragma mark -- Audience only

- (IBAction)onJoinLive:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onJoinLiveButton:)])
        [self.delegate onJoinLiveButton:sender];
}

- (IBAction)onFullScreen:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(onEnterFullScreenButton:)])
        [self.delegate onEnterFullScreenButton:sender];
}

#pragma mark -- Keyboard

//- (void)keyboardWillChangeFrame:(NSNotification *)notification
//{
//    if (!self.commentTextField.isEditing) {
//        return;
//    }
//
//    // 键盘弹出需要花费的时间
//    NSTimeInterval duration = [notification.userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
//    // 键盘的开始位置点
//    CGRect beginFrame = [notification.userInfo[UIKeyboardFrameBeginUserInfoKey] CGRectValue];
//    // 键盘的结束位置点
//    CGRect endFrame = [notification.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue];
//    // 计算键盘在 self.view 中的位置
//    endFrame = [self.view.window convertRect:endFrame toView:self.view];
//    if (CGRectEqualToRect(endFrame, CGRectZero)) {
//        return;
//    }
//
//    // 键盘弹出动画类型（开始/结束时快/慢)
//    NSUInteger animationCurve = [notification.userInfo[UIKeyboardAnimationCurveUserInfoKey] unsignedIntegerValue];
//
//    // 弹出键盘时，commentView 整体上移键盘高度
////    CGFloat chatInputOffset = CGRectGetMinY(endFrame) + self.bottomLayoutGuide.length - CGRectGetHeight(self.view.bounds);
////    if (chatInputOffset > 0) {
////        chatInputOffset = 0;
////    }
//
//    self.commentViewBottomToSuperviewBottom.constant = CGRectGetHeight(endFrame);     // 点击评论后，commentView 上移到键盘上方
//    [UIView animateWithDuration:duration
//                          delay:0.0
//                        options:animationCurve
//                     animations:^{
//                         [self.view layoutIfNeeded];
//                         self.commentView.alpha = !(beginFrame.origin.y < endFrame.origin.y);
//                     } completion:nil];
//}

- (void)keyboardWillShow:(NSNotification *)notification {
    if (!self.commentTextField.isEditing) {
        return;
    }
    
    // 键盘弹出需要花费的时间
    NSTimeInterval duration = [notification.userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
    // 键盘的开始位置点
    CGRect beginFrame = [notification.userInfo[UIKeyboardFrameBeginUserInfoKey] CGRectValue];
    // 键盘的结束位置点
    CGRect endFrame = [notification.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue];
    // 计算键盘在 self.view 中的位置
    endFrame = [self.view.window convertRect:endFrame toView:self.view];
    if (CGRectEqualToRect(endFrame, CGRectZero)) {
        return;
    }
    
    // 键盘弹出动画类型（开始/结束时快/慢)
    NSUInteger animationCurve = [notification.userInfo[UIKeyboardAnimationCurveUserInfoKey] unsignedIntegerValue];
    
    // 弹出键盘时，commentView 整体上移键盘高度
    //    CGFloat chatInputOffset = CGRectGetMinY(endFrame) + self.bottomLayoutGuide.length - CGRectGetHeight(self.view.bounds);
    //    if (chatInputOffset > 0) {
    //        chatInputOffset = 0;
    //    }
    
    self.commentViewBottomToSuperviewBottom.constant = CGRectGetHeight(endFrame);     // 点击评论后，commentView 上移到键盘上方
    [UIView animateWithDuration:duration
                          delay:0.0
                        options:animationCurve
                     animations:^{
                         [self.view layoutIfNeeded];
                         self.commentView.alpha = !(beginFrame.origin.y < endFrame.origin.y);
                     } completion:nil];
}

- (void)keyboardWillHide:(NSNotification *)notification {
    self.commentViewBottomToSuperviewBottom.constant = 0;     // 点击评论后，commentView 上移到键盘上方
    
    NSTimeInterval duration = [notification.userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
    
    // 键盘弹出动画类型（开始/结束时快/慢)
    NSUInteger animationCurve = [notification.userInfo[UIKeyboardAnimationCurveUserInfoKey] unsignedIntegerValue];
    
    [UIView animateWithDuration:duration
                          delay:0.0
                        options:animationCurve
                     animations:^{
                         [self.view layoutIfNeeded];
                         self.commentView.alpha = 0;
                     } completion:nil];
}

#pragma mark -- Time record

- (void)startTimeRecord
{
    if (!self.timeFormatter)
    {
        self.timeFormatter = [[NSDateFormatter alloc] init];
        self.timeFormatter.dateFormat = @"HH:mm:ss:SSS";
        self.timeFormatter.timeZone = [NSTimeZone timeZoneWithAbbreviation:@"GMT+0800"];
    }
    
    self.timeLabel.hidden = NO;
    
    if (!self.recordTimer)
    {
        self.recordTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/60 target:self selector:@selector(updateTimeLabel) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:self.recordTimer forMode:NSRunLoopCommonModes];
        
        [self.recordTimer fire];
    }
}

- (void)stopTimeRecord
{
    if (self.recordTimer != nil)
    {
        [self.recordTimer invalidate];
        self.recordTimer = nil;
    }
}

- (void)updateTimeLabel
{
    [self.timeLabel setText:[self.timeFormatter stringFromDate:[NSDate date]]];
}

#pragma mark -- rotate

//- (void)updateTopViewConstraints
//{
//    self.topViewHeight.constant = CGRectGetHeight(self.view.frame) - self.topViewBottomToSuperviewBottom.constant - self.commentViewHeight.constant;
//
//    [UIView animateWithDuration:0.1 animations:^{
//        [self.view layoutIfNeeded];
//    }];
//}
//
//- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
//{
//    [coordinator animateAlongsideTransition:^(id<UIViewControllerTransitionCoordinatorContext>  _Nonnull context) {
//
//        [self updateTopViewConstraints];
//    } completion:^(id<UIViewControllerTransitionCoordinatorContext>  _Nonnull context) {
//
//    }];
//
//    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
//}
//
//- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
//{
//    [self updateTopViewConstraints];
//
//    [super willAnimateRotationToInterfaceOrientation:toInterfaceOrientation duration:duration];
//}

#pragma mark -- Other

- (void)onTapView:(UIGestureRecognizer *)gesture
{
    if (self.commentTextField.isEditing)
    {
        [self.commentTextField resignFirstResponder];
    }
    else
    {
        if ([self.delegate respondsToSelector:@selector(onTapViewPoint:)])
            [self.delegate onTapViewPoint:[gesture locationInView:nil]];
    }
}

#pragma mark - Private

// 判断当前直播界面是主播或观众风格
- (void)setIsAudience:(BOOL)isAudience
{
    _isAudience = isAudience;
    
    if (self.isAudience)
    {
        self.anchorView.hidden = YES;
        self.audienceView.hidden = NO;
        self.audienceViewTopToSuperviewTop.constant = 20;   // 观众 toolView 上移
        self.shareButton.hidden = NO;
    }
    else
    {
        self.anchorView.hidden = NO;
        self.audienceView.hidden = YES;
        self.shareButton.hidden = NO;
    }
}

#pragma mark -- comment

- (void)updateLayout:(NSArray<ZegoRoomMessage *> *)messageList
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        for (ZegoRoomMessage *message in messageList)
        {
            if (message.category == ZEGO_CHAT)
                [self caculateLayout:@"" userName:message.fromUserName content:message.content];
            else if (message.category == ZEGO_LIKE)
            {
                //解析Content 内容
                NSData *contentData = [message.content dataUsingEncoding:NSUTF8StringEncoding];
                NSDictionary *contentDict = [NSJSONSerialization JSONObjectWithData:contentData options:0 error:nil];
                
                NSUInteger count = [contentDict[@"likeCount"] unsignedIntegerValue];
                if (count != 0)
                {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [self updateLikeAnimation:count];
                    });
                }
                [self caculateLayout:@"" userName:message.fromUserName content:NSLocalizedString(@"点赞了主播", nil)];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                [self.messageView reloadData];
                [self scrollTableViewToBottom];
            });
        }
    });
}

- (void)caculateLayout:(NSString *)userInfo userName:(NSString *)userName content:(NSString *)content
{
    if (userName.length == 0 || content.length == 0)
        return;
    
    CGFloat totalWidth = CGRectGetWidth(self.messageView.frame) - 20;
    
    NSMutableAttributedString *totalText = [[NSMutableAttributedString alloc] init];
    if (userInfo)
    {
        NSMutableAttributedString *userInfoString = [[NSMutableAttributedString alloc] initWithString:userInfo];
        userInfoString.yy_font = [UIFont systemFontOfSize:12.0];
        userInfoString.yy_color = [UIColor whiteColor];
        
        YYTextBorder *border = [YYTextBorder new];
        border.strokeColor = [UIColor redColor];
        border.fillColor = [UIColor redColor];
        border.cornerRadius = 1;
        border.lineJoin = kCGLineJoinBevel;
        border.insets = UIEdgeInsetsMake(0, 0, 2.5, 0);
        
        userInfoString.yy_textBackgroundBorder = border;
        [userInfoString addAttribute:NSBaselineOffsetAttributeName value:@(2) range:NSMakeRange(0, userInfo.length)];
        
        [totalText appendAttributedString:userInfoString];
    }
    
    NSMutableAttributedString *userNameString = [[NSMutableAttributedString alloc] initWithString:[NSString stringWithFormat:@" %@: ", userName]];
    
    userNameString.yy_font = [UIFont systemFontOfSize:16.0];
    userNameString.yy_color = [UIColor colorWithRed:253/255.0 green:181/255.0 blue:84/255.0 alpha:1.0];
    
    [totalText appendAttributedString:userNameString];
    
    NSMutableAttributedString *contentString = [[NSMutableAttributedString alloc] initWithString:content];
    contentString.yy_font = [UIFont systemFontOfSize:16.0];
    contentString.yy_color = [UIColor whiteColor];
    
    [totalText appendAttributedString:contentString];
    
    YYTextContainer *container = [YYTextContainer containerWithSize:CGSizeMake(totalWidth, CGFLOAT_MAX)];
    container.insets = UIEdgeInsetsMake(2, 10, 2, 10);
    
    YYTextLayout *textLayout = [YYTextLayout layoutWithContainer:container text:totalText];
    
    [self.liveMessageList addObject:textLayout];
    
}

- (void)scrollTableViewToBottom
{
    NSInteger lastItemIndex = [self.messageView numberOfRowsInSection:0] - 1;
    if (lastItemIndex < 0)
    {
        return;
    }
    
    NSIndexPath *lastIndexPath = [NSIndexPath indexPathForRow:lastItemIndex inSection:0];
    [self.messageView scrollToRowAtIndexPath:lastIndexPath atScrollPosition:UITableViewScrollPositionBottom animated:NO];
}

#pragma mark -- like

- (void)updateLikeAnimation:(NSUInteger)count
{
    if (count == 0)
        return;
    
    self.likeCount += count;
    if (self.timer == nil)
    {
        self.timer = [NSTimer scheduledTimerWithTimeInterval:0.3 target:self selector:@selector(onLikeAnimation) userInfo:nil repeats:YES];
    }
}

- (void)onLikeAnimation
{
    [self.likeShowView doLikeAnimation];
    self.likeCount -= 1;
    if (self.likeCount == 0)
    {
        [self.timer invalidate];
        self.timer = nil;
    }
}

#pragma mark - UITableView DataSource & delegate

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.liveMessageList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath
{
    NSString *cellID = @"liveMessageIdentifier";
    ZegoMessageTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellID];
    if (!cell)
    {
        cell = [[ZegoMessageTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellID];
    }
    
    if (indexPath.row >= self.liveMessageList.count)
        return cell;
    
    cell.layout = self.liveMessageList[indexPath.row];
    
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(nonnull NSIndexPath *)indexPath
{
    if (indexPath.row >= self.liveMessageList.count)
        return 0.0;
    
    YYTextLayout *layout = self.liveMessageList[indexPath.row];
    
    return layout.textBoundingSize.height;
}

- (BOOL)tableView:(UITableView *)tableView shouldHighlightRowAtIndexPath:(nonnull NSIndexPath *)indexPath
{
    return NO;
}

#pragma mark - UITextFieldDelegate

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [self onSendMessage:nil];
    
    return YES;
}

@end

