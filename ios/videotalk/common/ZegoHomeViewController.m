//
//  ZegoHomeViewController.m
//  VideoTalk
//
//  Created by summery on 23/10/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import "ZegoHomeViewController.h"
#import "ZegoSetting.h"
#import "ZegoTalkViewController.h"

@interface ZegoHomeViewController ()

@property (weak, nonatomic) IBOutlet UITextField *sessionIDText;
@property (weak, nonatomic) IBOutlet UIButton *startTalkButton;

@property (nonatomic, strong) UIColor *defaultButtonColor;
@property (nonatomic, strong) UIColor *disableButtonColor;

@property (nonatomic, strong) UITapGestureRecognizer *tapGesture;

@end

@implementation ZegoHomeViewController

#pragma mark - Life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.defaultButtonColor = [self.startTalkButton backgroundColor];
    self.disableButtonColor = [UIColor lightGrayColor];
    
    self.startTalkButton.enabled = NO;
    self.startTalkButton.backgroundColor = self.disableButtonColor;
    self.startTalkButton.layer.cornerRadius = 6.0;
    
    
    self.sessionIDText.clearButtonMode = UITextFieldViewModeWhileEditing;
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textFieldTextDidChange) name:UITextFieldTextDidChangeNotification object:self.sessionIDText];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    NSString *title = [NSString stringWithFormat:NSLocalizedString(@"VideoTalk(%@)", nil), [ZegoSetting sharedInstance].appTypeList[[ZegoSetting sharedInstance].appType]];
    self.navigationItem.title = title;
    
    // 兼容 11.2 的 Bug, 参考：https://stackoverflow.com/questions/47754472/ios-uinavigationbar-button-remains-faded-after-segue-back
    [self.navigationItem.rightBarButtonItem setEnabled:NO];
    [self.navigationItem.rightBarButtonItem setEnabled:YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Event response

- (void)textFieldTextDidChange {
    if (self.sessionIDText.text.length > 0) {
        self.startTalkButton.enabled = YES;
        self.startTalkButton.backgroundColor = self.defaultButtonColor;
    } else {
        self.startTalkButton.enabled = NO;
        self.startTalkButton.backgroundColor = self.disableButtonColor;
    }
}

- (IBAction)onStartTalk:(id)sender {
    NSCharacterSet *set = [NSCharacterSet whitespaceAndNewlineCharacterSet];
    NSString *isString = [self.sessionIDText.text stringByTrimmingCharactersInSet:set];
    if (isString.length == 0) {
        [self showAlert:@"提示" message:@"房间 ID 不可为空格，请重新输入！"];
        return;
    }
    
    if (self.sessionIDText.text.length != 0) {
        ZegoTalkViewController *talkController = [[ZegoTalkViewController alloc] init];
        talkController.roomID = self.sessionIDText.text;
        [self presentViewController:talkController animated:YES completion:nil];
    }
}

- (void)onTapTableView:(UIGestureRecognizer *)gesture
{
    [self.view endEditing:YES];
}

- (void)showAlert:(NSString *)title message:(NSString *)message {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title
                                                                             message:message
                                                                      preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *confirm = [UIAlertAction actionWithTitle:@"确定"
                                                      style:UIAlertActionStyleDefault
                                                    handler:^(UIAlertAction * _Nonnull action) {
                                                        
                                                    }];
    
    [alertController addAction:confirm];
    [self presentViewController:alertController animated:YES completion:nil];
}

#pragma mark - UITextFieldDelegate

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    if (self.tapGesture == nil) {
        self.tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapTableView:)];
    }
    
    [self.view addGestureRecognizer:self.tapGesture];
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
    if (text.length == 0) {
        self.startTalkButton.enabled = NO;
        self.startTalkButton.backgroundColor = self.disableButtonColor;
    } else {
        self.startTalkButton.enabled = YES;
        self.startTalkButton.backgroundColor = self.defaultButtonColor;
    }
    
    if ([text isEqualToString:@"\n"]) {
        [textView resignFirstResponder];
        return NO;
    }
    
    return YES;
}

@end
