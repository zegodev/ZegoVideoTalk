//
//  ZegoTalkToolViewController.m
//  VideoTalk
//
//  Created by summery on 24/10/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "ZegoTalkToolViewController.h"

@interface ZegoTalkToolViewController ()

@end

@implementation ZegoTalkToolViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor clearColor];
    
    self.closeButton.layer.cornerRadius = self.closeButton.frame.size.width / 2;
    self.logButton.layer.cornerRadius = self.logButton.frame.size.width / 2;
    self.muteButton.layer.cornerRadius = self.muteButton.frame.size.width / 2;
    self.cameraButton.layer.cornerRadius = self.cameraButton.frame.size.width / 2;
    self.micButton.layer.cornerRadius = self.micButton.frame.size.width / 2;
    self.switchCameraButton.layer.cornerRadius = self.switchCameraButton.frame.size.width / 2;

    UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapView:)];
    [self.view addGestureRecognizer:tapGestureRecognizer];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Event response

- (IBAction)onCameraButton:(id)sender {
    if ([self.delegate respondsToSelector:@selector(onCameraButton:)]) {
        [self.delegate onCameraButton:sender];
    }
}

- (IBAction)onMicButton:(id)sender {
    if ([self.delegate respondsToSelector:@selector(onMicButton:)]) {
        [self.delegate onMicButton:sender];
    }
}

- (IBAction)onMuteButton:(id)sender {
    if ([self.delegate respondsToSelector:@selector(onMuteButton:)]) {
        [self.delegate onMuteButton:sender];
    }
}

- (IBAction)onLogButton:(id)sender {
    if ([self.delegate respondsToSelector:@selector(onLogButton:)]) {
        [self.delegate onLogButton:sender];
    }
}

- (IBAction)onCloseButton:(id)sender {
    if ([self.delegate respondsToSelector:@selector(onCloseButton:)]) {
        [self.delegate onCloseButton:sender];
    }
}

- (IBAction)onSwitchCameraButton:(id)sender {
    if ([self.delegate respondsToSelector:@selector(onSwitchCameraButton:)]) {
        [self.delegate onSwitchCameraButton:sender];
    }
}

- (void)onTapView:(UIGestureRecognizer *)gesture
{
    if ([self.delegate respondsToSelector:@selector(onTapViewPoint:)]) {
        [self.delegate onTapViewPoint:[gesture locationInView:nil]];
    }
}

@end
