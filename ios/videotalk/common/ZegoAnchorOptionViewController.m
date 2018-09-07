//
//  ZegoAnchorOptionViewController.m
//  
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017年 ZEGO. All rights reserved.
//

#import "ZegoAnchorOptionViewController.h"
#import "ZegoSetting.h"
#import "ZegoManager.h"

#pragma mark - ZegoAnchorOptionSwitchCell

@implementation ZegoAnchorOptionSwitchCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        self.switchButton = [[UISwitch alloc] init];
        [self.switchButton setTranslatesAutoresizingMaskIntoConstraints:NO];
        [self.contentView addSubview:self.switchButton];
        
        self.titleLabel = [[UILabel alloc] init];
        [self.titleLabel setTranslatesAutoresizingMaskIntoConstraints:NO];
        [self.contentView addSubview:self.titleLabel];
        
        NSLayoutConstraint *titleLabelLeft = [NSLayoutConstraint constraintWithItem:self.titleLabel attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeLeft multiplier:1 constant:10];
        NSLayoutConstraint *titleLabelWidth = [NSLayoutConstraint constraintWithItem:self.titleLabel attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil     attribute:NSLayoutAttributeNotAnAttribute multiplier:1 constant:160];
        NSLayoutConstraint *titleLabelCenterY = [NSLayoutConstraint constraintWithItem:self.titleLabel attribute:NSLayoutAttributeCenterY relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeCenterY multiplier:1 constant:0];
        NSLayoutConstraint *switchButtonRight = [NSLayoutConstraint constraintWithItem:self.switchButton attribute:NSLayoutAttributeRight relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeRight multiplier:1 constant:-10];
        NSLayoutConstraint *switchButtonCenterY = [NSLayoutConstraint constraintWithItem:self.switchButton attribute:NSLayoutAttributeCenterY relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeCenterY multiplier:1 constant:0];
        
        NSArray *constraints = [NSArray arrayWithObjects:titleLabelLeft, titleLabelWidth, titleLabelCenterY, switchButtonRight, switchButtonCenterY, nil];
        [self.contentView addConstraints:constraints];
    }
    
    return self;
}


@end

#pragma mark - ZegoAnchorOptionPickerCell

@implementation ZegoAnchorOptionPickerCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        self.pickerView = [[UIPickerView alloc] init];
        [self.pickerView setTranslatesAutoresizingMaskIntoConstraints:NO];
        [self.contentView addSubview:self.pickerView];
        
        NSLayoutConstraint *pickerTop = [NSLayoutConstraint constraintWithItem:self.pickerView attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeTop multiplier:1.0 constant:0];
        NSLayoutConstraint *pickerBottom = [NSLayoutConstraint constraintWithItem:self.pickerView attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeBottom multiplier:1.0 constant:0];
        NSLayoutConstraint *pickerLeft = [NSLayoutConstraint constraintWithItem:self.pickerView attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeLeft multiplier:1.0 constant:30];
        NSLayoutConstraint *pickerRight = [NSLayoutConstraint constraintWithItem:self.pickerView attribute:NSLayoutAttributeRight relatedBy:NSLayoutRelationEqual toItem:self.contentView attribute:NSLayoutAttributeRight multiplier:1.0 constant:-30];
        
        NSArray *constraints = [NSArray arrayWithObjects:pickerTop, pickerBottom, pickerLeft, pickerRight, nil];
        [self.contentView addConstraints:constraints];
    }
    return self;
}

@end

#pragma mark - ZegoAnchotOptionViewController

@interface ZegoAnchorOptionViewController () <UIPickerViewDelegate, UIPickerViewDataSource, UITableViewDataSource, UITableViewDelegate>

@property (nonatomic, copy) NSArray *beautifyList;
@property (nonatomic, copy) NSArray *filterList;

@property (nonatomic, strong) UIPickerView *beautifyPicker;
@property (nonatomic, strong) UIPickerView *filterPicker;

@property (nonatomic, strong) UITableView *tableView;

@property (nonatomic, assign, readonly) BOOL useFrontCamera;
@property (nonatomic, assign, readonly) BOOL enableMicrophone;
@property (nonatomic, assign, readonly) BOOL enableTorch;
@property (nonatomic, assign, readonly) NSUInteger beautifyRow;
@property (nonatomic, assign, readonly) NSUInteger filterRow;
@property (nonatomic, assign, readonly) BOOL enableCamera;
@property (nonatomic, assign, readonly) BOOL enableAux;

@property (nonatomic, assign, readonly) BOOL enablePreviewMirror;
@property (nonatomic, assign, readonly) BOOL enableCaptureMirror;
@property (nonatomic, assign, readonly) BOOL enableLoopback;

@end

@implementation ZegoAnchorOptionViewController

#pragma mark -- Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.beautifyList = [ZegoSetting sharedInstance].beautifyList;
    self.filterList = [ZegoSetting sharedInstance].filterList;
    
    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onClose:)];
    [self.view addGestureRecognizer:tapGesture];
    
    self.tableView = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStyleGrouped];
    [self.tableView setTranslatesAutoresizingMaskIntoConstraints:NO];
    self.tableView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:self.tableView];
    self.tableView.layer.cornerRadius = 5.0;
    self.tableView.alpha = 0.7;
    self.tableView.layer.borderColor = [UIColor lightGrayColor].CGColor;
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    
    NSLayoutConstraint *tableViewTop = [NSLayoutConstraint constraintWithItem:self.tableView attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:self.view attribute:NSLayoutAttributeTop multiplier:1 constant:100];
    NSLayoutConstraint *tableViewLeft = [NSLayoutConstraint constraintWithItem:self.tableView attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:self.view     attribute:NSLayoutAttributeLeft multiplier:1 constant:30];
    NSLayoutConstraint *tableViewRight= [NSLayoutConstraint constraintWithItem:self.tableView attribute:NSLayoutAttributeRight relatedBy:NSLayoutRelationEqual toItem:self.view     attribute:NSLayoutAttributeRight multiplier:1 constant:-30];
    NSLayoutConstraint *tableViewBottom = [NSLayoutConstraint constraintWithItem:self.tableView attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:self.view attribute:NSLayoutAttributeBottom multiplier:1 constant:0];

    NSArray *constraints = [NSArray arrayWithObjects:tableViewTop, tableViewLeft, tableViewRight, tableViewBottom, nil];
    [self.view addConstraints:constraints];

    [self.tableView registerClass:[ZegoAnchorOptionSwitchCell class] forCellReuseIdentifier:@"anchorSwitchCellIdentifier"];
    [self.tableView registerClass:[ZegoAnchorOptionPickerCell class] forCellReuseIdentifier:@"anchorPickerCellIdentifier"];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onHeadSetStateChange:) name:kHeadSetStateChangeNotification object:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

#pragma mark -- Event response

- (void)onClose:(UITapGestureRecognizer *)recognizer {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)onHeadSetStateChange:(NSNotification *)notification
{
    [self.tableView reloadData];
}

- (NSUInteger)filterRow
{
    if ([self.delegate respondsToSelector:@selector(onGetSelectedFilter)])
        return [self.delegate onGetSelectedFilter];
    
    return 0;
}

- (BOOL)enableCamera
{
    if ([self.delegate respondsToSelector:@selector(onGetEnableCamera)])
        return [self.delegate onGetEnableCamera];
    
    return NO;
}

- (BOOL)enableAux
{
    if ([self.delegate respondsToSelector:@selector(onGetEnableAux)])
        return [self.delegate onGetEnableAux];
    
    return NO;
}

- (BOOL)enablePreviewMirror
{
    if ([self.delegate respondsToSelector:@selector(onGetEnablePreviewMirror)])
        return [self.delegate onGetEnablePreviewMirror];
    
    return NO;
}

- (BOOL)enableCaptureMirror
{
    if ([self.delegate respondsToSelector:@selector(onGetEnableCaptureMirror)])
        return [self.delegate onGetEnableCaptureMirror];
    
    return NO;
}

- (BOOL)enableLoopback
{
    if ([self.delegate respondsToSelector:@selector(onGetEnableLoopback)])
        return [self.delegate onGetEnableLoopback];
    
    return NO;
}

- (void)toggleFrontCamera:(id)sender
{
    UISwitch *switchCamera = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onUseFrontCamera:)])
        [self.delegate onUseFrontCamera:switchCamera.on];
    
    [self.tableView reloadData];
}

- (void)toggleMicrophone:(id)sender
{
    UISwitch *switchMicrophone = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onEnableMicrophone:)])
        [self.delegate onEnableMicrophone:switchMicrophone.on];
}

- (void)toggleTorch:(id)sender
{
    UISwitch *switchTorch = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onEnableTorch:)])
        [self.delegate onEnableTorch:switchTorch.on];
}

- (void)toggleCamera:(id)sender
{
    UISwitch *switchTorch = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onEnableCamera:)])
        [self.delegate onEnableCamera:switchTorch.on];
    
    [self.tableView reloadData];
}

- (void)toggleAux:(id)sender
{
    UISwitch *switchAux = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onEnableAux:)])
        [self.delegate onEnableAux:switchAux.on];
}

- (void)togglePreviewMirror:(id)sender
{
    UISwitch *switchPreview = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onEnablePreviewMirror:)])
        [self.delegate onEnablePreviewMirror:switchPreview.on];
}

- (void)toggleCaptureMirror:(id)sender
{
    UISwitch *switchCapture = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onEnableCaptureMirror:)])
        [self.delegate onEnableCaptureMirror:switchCapture.on];
}

- (void)toggleLoopback:(id)sender
{
    UISwitch *switchLoop = (UISwitch *)sender;
    if ([self.delegate respondsToSelector:@selector(onEnableLoopback:)])
        [self.delegate onEnableLoopback:switchLoop.on];
}

#pragma mark -- UITableViewDataSource、UITableViewDelegate

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 3; // TODO: 先写死调通，后面改成可配置
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (section == 0) {
        return 8;
    }
    
    return 1;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 0)
        return 44.0;
    else
        return 132.0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (section == 0)
        return NSLocalizedString(@"设置", nil);
    else if (section == 1)
        return NSLocalizedString(@"美颜", nil);
    else if (section == 2)
        return NSLocalizedString(@"滤镜", nil);
    
    return nil;
}

- (BOOL)tableView:(UITableView *)tableView shouldHighlightRowAtIndexPath:(NSIndexPath *)indexPath {
    return NO;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.section == 0)
    {
        ZegoAnchorOptionSwitchCell *cell = (ZegoAnchorOptionSwitchCell *)[tableView dequeueReusableCellWithIdentifier:@"anchorSwitchCellIdentifier" forIndexPath:indexPath];
        if (indexPath.row == 0)
        {
            cell.titleLabel.text = NSLocalizedString(@"启用摄像头", nil);
            cell.switchButton.on = self.enableCamera;
            cell.switchButton.enabled = YES;
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(toggleCamera:) forControlEvents:UIControlEventValueChanged];
        }
        else if (indexPath.row == 1)
        {
            cell.titleLabel.text = NSLocalizedString(@"前置摄像头", nil);
            cell.switchButton.on = self.useFrontCamera;
            cell.switchButton.enabled = YES;
            if (self.enableCamera == NO)
                cell.switchButton.enabled = NO;
            
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(toggleFrontCamera:) forControlEvents:UIControlEventValueChanged];
        }
        else if (indexPath.row == 2)
        {
            cell.titleLabel.text = NSLocalizedString(@"预览切换镜像", nil);
            cell.switchButton.on = self.enablePreviewMirror;
            cell.switchButton.enabled = YES;
            if (self.enableCamera == NO)
                cell.switchButton.enabled = NO;
            if (self.useFrontCamera == NO)
                cell.switchButton.enabled = NO;
            
//            if ([ZegoManager recordTime])
//                cell.switchButton.enabled = NO;
            
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(togglePreviewMirror:) forControlEvents:UIControlEventValueChanged];
        }
        else if (indexPath.row == 3)
        {
            cell.titleLabel.text = NSLocalizedString(@"采集镜像", nil);
            cell.switchButton.on = self.enableCaptureMirror;
            cell.switchButton.enabled = YES;
            if (self.enableCamera == NO)
                cell.switchButton.enabled = NO;
            
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(toggleCaptureMirror:) forControlEvents:UIControlEventValueChanged];
        }
        else if (indexPath.row == 4)
        {
            cell.titleLabel.text = NSLocalizedString(@"麦克风", nil);
            cell.switchButton.on = self.enableMicrophone;
            cell.switchButton.enabled = YES;
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(toggleMicrophone:) forControlEvents:UIControlEventValueChanged];
        }
        else if (indexPath.row == 5)
        {
            cell.titleLabel.text = NSLocalizedString(@"手电筒", nil);
            cell.switchButton.on = self.enableTorch;
            cell.switchButton.enabled = YES;
            if (self.enableCamera == NO)
                cell.switchButton.enabled = NO;
            
            if (self.useFrontCamera)
                cell.switchButton.enabled = NO;
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(toggleTorch:) forControlEvents:UIControlEventValueChanged];
        }
        else if (indexPath.row == 6)
        {
            cell.titleLabel.text = NSLocalizedString(@"混音", nil);
            cell.switchButton.on = self.enableAux;
            cell.switchButton.enabled = YES;
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(toggleAux:) forControlEvents:UIControlEventValueChanged];
        }
        else if (indexPath.row == 7)
        {
            cell.titleLabel.text = NSLocalizedString(@"采集监听", nil);
            cell.switchButton.on = self.enableLoopback;
            cell.switchButton.enabled = YES;
            
            if (![ZegoSetting sharedInstance].useHeadSet)
            {
                cell.switchButton.enabled = NO;
            }
            
            [cell.switchButton removeTarget:self action:NULL forControlEvents:UIControlEventValueChanged];
            [cell.switchButton addTarget:self action:@selector(toggleLoopback:) forControlEvents:UIControlEventValueChanged];
        }
        return cell;
    }
    else
    {
        ZegoAnchorOptionPickerCell *cell = (ZegoAnchorOptionPickerCell *)[tableView dequeueReusableCellWithIdentifier:@"anchorPickerCellIdentifier" forIndexPath:indexPath];
        cell.pickerView.dataSource = self;
        cell.pickerView.delegate = self;
        
        if (indexPath.section == 1)
        {
            self.beautifyPicker = cell.pickerView;
            [cell.pickerView selectRow:self.beautifyRow inComponent:0 animated:NO];
        }
        else
        {
            self.filterPicker = cell.pickerView;
            [cell.pickerView selectRow:self.filterRow inComponent:0 animated:NO];
        }
        
        return cell;
    }
}

#pragma mark -- UIPickerViewDelegate, UIPickerViewDataSource

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    if (pickerView == self.beautifyPicker) {
        return self.beautifyList.count;
    } else {
        return self.filterList.count;
    }
}

- (NSString*)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    NSArray *dataList = nil;
    if (pickerView == self.beautifyPicker) {
        dataList = self.beautifyList;
    } else {
        dataList = _filterList;
    }
    
    if (row >= dataList.count) {
        return @"Error";
    }
    
    return [dataList objectAtIndex:row];
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    if (pickerView == self.beautifyPicker)
    {
        if ([self.delegate respondsToSelector:@selector(onSelectedBeautify:)])
            [self.delegate onSelectedBeautify:row];
    }
    else
    {
        if ([self.delegate respondsToSelector:@selector(onSelectedFilter:)])
            [self.delegate onSelectedFilter:row];
    }
}

#pragma mark -- Getter

- (BOOL)useFrontCamera
{
    if ([self.delegate respondsToSelector:@selector(onGetUseFrontCamera)])
        return [self.delegate onGetUseFrontCamera];
    
    return NO;
}

- (BOOL)enableMicrophone
{
    if ([self.delegate respondsToSelector:@selector(onGetEnableMicrophone)])
        return [self.delegate onGetEnableMicrophone];
    
    return NO;
}

- (BOOL)enableTorch
{
    if ([self.delegate respondsToSelector:@selector(onGetEnableTorch)])
        return [self.delegate onGetEnableTorch];
    
    return NO;
}

- (NSUInteger)beautifyRow
{
    if ([self.delegate respondsToSelector:@selector(onGetSelectedBeautify)])
        return [self.delegate onGetSelectedBeautify];
    
    return 0;
}

@end

