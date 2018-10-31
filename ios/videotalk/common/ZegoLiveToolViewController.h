//
//  ZegoAnchorToolViewController.h
//
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "YYText/YYText.h"
#import "ZegoManager.h"

@protocol ZegoLiveToolViewControllerDelegate <NSObject>

@optional

- (void)onSendComment:(NSString *)comment;
- (void)onSendLike;

- (void)onOptionButton:(id)sender;
- (void)onMutedButton:(id)sender;
- (void)onLogButton:(id)sender;
- (void)onCloseButton:(id)sender;

- (void)onShareButton:(id)sender;
- (void)onStopPublishButton:(id)sender;

- (void)onJoinLiveButton:(id)sender;
- (void)onEnterFullScreenButton:(id)sender;

- (void)onTapViewPoint:(CGPoint)point;

@end

@interface ZegoLiveToolViewController : UIViewController

@property (nonatomic, weak) id<ZegoLiveToolViewControllerDelegate> delegate;

@property (nonatomic, assign) BOOL isAudience;

// 主播推流界面操作
@property (nonatomic, weak) IBOutlet UIButton *optionButton;        // 直播设置
@property (nonatomic, weak) IBOutlet UIButton *stopPublishButton;   // 停止直播
@property (nonatomic, weak) IBOutlet UIButton *mutedButton;         // 静音
@property (nonatomic, weak) IBOutlet UIButton *shareButton;         // 分享

// 观众拉流界面操作
@property (nonatomic, weak) IBOutlet UIButton *joinLiveOptionButton;    // 直播设置（连麦后可以设置）
@property (nonatomic, weak) IBOutlet UIButton *joinLiveButton;          // 请求连麦
@property (nonatomic, weak) IBOutlet UIButton *playMutedButton;         // 静音
@property (nonatomic, weak) IBOutlet UIButton *fullScreenButton;        // 全屏
@property (nonatomic, weak) IBOutlet UILabel *renderLabel;              // render

@property (nonatomic, weak) IBOutlet UILabel *timeLabel;

//- (void)updateLayout:(NSArray<ZegoComment *> *)commentList;
- (void)updateLayout:(NSArray<ZegoRoomMessage *> *)messageList;

- (void)updateLikeAnimation:(NSUInteger)count;

- (void)startTimeRecord;
- (void)stopTimeRecord;

@end

