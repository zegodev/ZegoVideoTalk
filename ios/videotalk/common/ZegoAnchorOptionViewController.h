//
//  ZegoAnchorOptionViewController.h
//
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017年 ZEGO. All rights reserved.
//

#import <UIKit/UIKit.h>

#define kHeadSetStateChangeNotification     @"headSetStateChange"

@protocol ZegoAnchorOptionDelegate <NSObject>

- (void)onUseFrontCamera:(BOOL)use;
- (void)onEnableMicrophone:(BOOL)enabled;
- (void)onEnableTorch:(BOOL)enable;
- (void)onSelectedBeautify:(NSInteger)row;
- (void)onSelectedFilter:(NSInteger)row;
- (void)onEnableCamera:(BOOL)enabled;
- (void)onEnableAux:(BOOL)enabled;

- (void)onEnablePreviewMirror:(BOOL)enabled;
- (void)onEnableCaptureMirror:(BOOL)enable;
- (void)onEnableLoopback:(BOOL)enable;

- (BOOL)onGetUseFrontCamera;
- (BOOL)onGetEnableMicrophone;
- (BOOL)onGetEnableTorch;
- (NSInteger)onGetSelectedBeautify;
- (NSInteger)onGetSelectedFilter;
- (BOOL)onGetEnableCamera;
- (BOOL)onGetEnableAux;
- (BOOL)onGetEnablePreviewMirror;
- (BOOL)onGetEnableCaptureMirror;
- (BOOL)onGetEnableLoopback;

@end

@interface ZegoAnchorOptionSwitchCell : UITableViewCell

@property (strong, nonatomic) UISwitch *switchButton;
@property (strong, nonatomic) UILabel *titleLabel;

@end

@interface ZegoAnchorOptionPickerCell : UITableViewCell

@property (strong, nonatomic) UIPickerView *pickerView;

@end

@interface ZegoAnchorOptionViewController : UIViewController

@property (nonatomic, weak) id<ZegoAnchorOptionDelegate> delegate;

@end

