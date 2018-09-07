//
//  ZegoRoomInfo.h
//
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ZegoRoomInfo : NSObject

@property (nonatomic, copy) NSString *roomID;
@property (nonatomic, copy) NSString *roomName;
@property (nonatomic, copy) NSString *anchorID;
@property (nonatomic, copy) NSString *anchorName;
@property (nonatomic, strong) NSMutableArray *streamInfo;   // streamID 列表

@end

