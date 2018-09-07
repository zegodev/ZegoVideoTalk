//
//  ZegoTalkToolViewController.h
//  VideoTalk
//
//  Created by summery on 24/10/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ZegoTalkToolViewControllerDelegate <NSObject>

@optional

- (void)onCloseButton:(id)sender;
- (void)onLogButton:(id)sender;
- (void)onMuteButton:(id)sender;
- (void)onMicButton:(id)sender;
- (void)onCameraButton:(id)sender;
- (void)onSwitchCameraButton:(id)sender;

- (void)onTapViewPoint:(CGPoint)point;

@end

@interface ZegoTalkToolViewController : UIViewController

@property (nonatomic, weak) id<ZegoTalkToolViewControllerDelegate> delegate;

@property (weak, nonatomic) IBOutlet UIButton *cameraButton;
@property (weak, nonatomic) IBOutlet UIButton *micButton;
@property (weak, nonatomic) IBOutlet UIButton *muteButton;
@property (weak, nonatomic) IBOutlet UIButton *logButton;
@property (weak, nonatomic) IBOutlet UIButton *closeButton;
@property (weak, nonatomic) IBOutlet UIButton *switchCameraButton;

@end
