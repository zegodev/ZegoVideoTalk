//
//  ZegoManager.m
//
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import "ZegoManager.h"
#import "ZegoSetting.h"

@implementation ZegoManager

static ZegoLiveRoomApi *_apiInstance = nil;

+ (ZegoLiveRoomApi *)api {
    if (_apiInstance == nil) {
        
#ifdef VIDEOTALK
        // 业务类型为：实时音视频类型
        if ([ZegoSetting sharedInstance].appType == ZegoAppTypeRTC)
            [ZegoLiveRoomApi setBusinessType: 0];
        else
            [ZegoLiveRoomApi setBusinessType: 2];
        
        if ([[ZegoSetting iOSDeviceType] isEqualToString:IPAD_PRO_129_2ND]) {
            [ZegoLiveRoomApi setAudioDeviceMode:ZEGOAPI_AUDIO_DEVICE_MODE_GENERAL]; // 关闭系统回声消除
        }
#endif
        
        // 测试环境
        [ZegoLiveRoomApi setUseTestEnv:[ZegoSetting sharedInstance].useTestEnv];
        
        // 调试信息
#ifdef DEBUG
        [ZegoLiveRoomApi setVerbose:YES];
#endif
        
#ifdef VIDEOLIVE
        // 外部渲染
        [ZegoLiveRoomApi enableExternalRender:[ZegoSetting sharedInstance].useExternalRender];
        
        // 初始化外部采集和滤镜
        [[ZegoSetting sharedInstance] setupVideoCaptureDevice];
        [[ZegoSetting sharedInstance] setupVideoFilter];
#endif
        
        // 初始化用户信息
        [ZegoLiveRoomApi setUserID:[ZegoSetting sharedInstance].userID userName:[ZegoSetting sharedInstance].userName];
        
        // 初始化 SDK 实例
        _apiInstance = [[ZegoLiveRoomApi alloc] initWithAppID:[ZegoSetting sharedInstance].appID appSignature:[ZegoSetting sharedInstance].appSign];
        
        // 初始化硬件编解码配置
#if TARGET_OS_SIMULATOR
        [ZegoSetting sharedInstance].useHardwareDecode = NO;
        [ZegoSetting sharedInstance].useHardwareEncode = NO;
#else
        [ZegoSetting sharedInstance].useHardwareDecode = YES;
        [ZegoSetting sharedInstance].useHardwareEncode = YES;
#endif
        
        [ZegoLiveRoomApi requireHardwareDecoder:[ZegoSetting sharedInstance].useHardwareDecode];
        [ZegoLiveRoomApi requireHardwareEncoder:[ZegoSetting sharedInstance].useHardwareEncode];
        
#ifdef VIDEOLIVE
        // 初始化流量控制配置
        if ([ZegoSetting sharedInstance].appType == ZegoAppTypeUDP || [ZegoSetting sharedInstance].appType == ZegoAppTypeI18N) {
            [_apiInstance enableTrafficControl:YES properties:ZEGOAPI_TRAFFIC_FPS | ZEGOAPI_TRAFFIC_RESOLUTION];
        }
#endif
        
#ifdef VIDEOTALK
//        [_apiInstance setLatencyMode:ZEGOAPI_LATENCY_MODE_LOW3];
        if ([ZegoSetting sharedInstance].appType == ZegoAppTypeRTC) {
            [_apiInstance setLatencyMode:ZEGOAPI_LATENCY_MODE_LOW3];
        } else {
            [_apiInstance setLatencyMode:ZEGOAPI_LATENCY_MODE_LOW];
        }
        
        if ([[ZegoSetting iOSDeviceType] isEqualToString:IPAD_PRO_129_2ND]) {
            [_apiInstance enableAEC:YES]; // 开启软件回声消除
            [_apiInstance enableNoiseSuppress:YES]; // 开启噪声抑制
        }
#endif
        
    }

    return _apiInstance;
}

+ (void)releaseApi {
    _apiInstance = nil;
}

@end

