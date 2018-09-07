//
//  ZegoManager.h
//
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <ZegoLiveRoom/ZegoLiveRoom.h>

//#define VIDEOLIVE       // 编译 VideoLive 使用该宏
#define VIDEOTALK     // 编译 VideoTalk 使用该宏

@interface ZegoManager : NSObject

// 获取 ZegoLiveRoomAPi 单例对象
+ (ZegoLiveRoomApi *)api;

// 释放 api 对象
+ (void)releaseApi;

@end

