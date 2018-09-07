//
//  ZegoInstrument.h
//  
//
//  Created by summery on 13/09/2017.
//  Copyright © 2017 ZEGO. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface ZegoInstrument : NSObject

+ (instancetype)shareInstance;

- (float)getCPUUsage;
- (float)getMemoryUsage;
- (float)getBatteryLevel;

@end
