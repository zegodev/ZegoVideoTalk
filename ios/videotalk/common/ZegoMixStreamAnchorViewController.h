//
//  ZegoMixStreamViewController.h
// 
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import "ZegoLiveViewController.h"

@interface ZegoMixStreamAnchorViewController : ZegoLiveViewController
//直播标题
@property (nonatomic, copy) NSString *liveTitle;
//预览的界面view
@property (nonatomic, strong) UIView *publishView;


@end

