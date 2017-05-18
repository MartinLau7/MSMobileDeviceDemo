//
//  MSMobileDeviceManager.h
//  MSMobileDeviceDemo
//
//  Created by MartinLau on 2017/5/18.
//  Copyright © 2017年 Martin Studio. All rights reserved.
//

#import <Foundation/Foundation.h>

extern NSString * const kMSDeviceConnectedNotification;              //装置连接
extern NSString * const kMSDeviceDisConnecteNotification;            //装置断开

@interface MSMobileDeviceManager : NSObject

+ (MSMobileDeviceManager *)sharedManager;

@end
