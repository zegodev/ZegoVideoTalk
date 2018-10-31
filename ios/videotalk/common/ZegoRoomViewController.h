//
//  ZegoRoomViewController.h
//  
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 zego. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ZegoRoomTableViewCell : UITableViewCell

@end

@protocol ZegoRoomViewControllerDelegate <NSObject>

- (void)onRefreshRoomListFinished;

@end

@interface ZegoRoomViewController : UIViewController <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, weak) id<ZegoRoomViewControllerDelegate> delegate;

- (void)refreshRoomList;

@end
