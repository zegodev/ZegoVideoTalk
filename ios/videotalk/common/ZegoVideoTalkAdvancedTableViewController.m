//
//  ZegoVideoTalkAdvancedTableViewController.m
//  VideoTalk
//
//  Created by summery on 24/10/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "ZegoVideoTalkAdvancedTableViewController.h"
#import "ZegoManager.h"
#import "ZegoSetting.h"

@interface ZegoVideoTalkAdvancedTableViewController () <UIPickerViewDelegate, UIPickerViewDataSource>

@property (weak, nonatomic) IBOutlet UISwitch *encodeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *decodeSwitch;
@property (weak, nonatomic) IBOutlet UIPickerView *streamLayerTypePicker;

@end

@implementation ZegoVideoTalkAdvancedTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self updateUIView];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
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

- (void)updateUIView {
    self.encodeSwitch.on =  [ZegoSetting sharedInstance].useHardwareEncode;
    self.decodeSwitch.on =  [ZegoSetting sharedInstance].useHardwareDecode;
    [self.streamLayerTypePicker selectRow:[ZegoSetting sharedInstance].videoCodecType inComponent:0 animated:NO];
}

#pragma mark - UIPickerViewDelegate & UIPickerViewDataSource

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

- (NSInteger) pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    return [ZegoSetting sharedInstance].videoCodecTypeList.count;
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    if (row >= [ZegoSetting sharedInstance].videoCodecTypeList.count) {
        return @"Error";
    }
    
    return [[ZegoSetting sharedInstance].videoCodecTypeList objectAtIndex:row];
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    if (row >= [ZegoSetting sharedInstance].videoCodecTypeList.count) {
        return;
    }
    
    [ZegoSetting sharedInstance].videoCodecType = row;
}


@end
