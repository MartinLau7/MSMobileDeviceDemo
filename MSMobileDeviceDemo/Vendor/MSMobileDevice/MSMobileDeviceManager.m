//
//  MSMobileDeviceManager.m
//  MSMobileDeviceDemo
//
//  Created by MartinLau on 2017/5/18.
//  Copyright © 2017年 Martin Studio. All rights reserved.
//

#import "MSMobileDevice.h"
#import "MSMobileDeviceManager.h"


NSString * const kMSDeviceConnectedNotification = @"MSDeviceConnectedNotification";
NSString * const kMSDeviceDisConnecteNotification = @"MSDeviceDisConnecteNotification";


@interface MSMobileDeviceManager () {
@private
    id _listener;
    BOOL _subscribed;
    am_device_notification _notification;
    BOOL _waitingInRunLoop;
    
}

@end


@implementation MSMobileDeviceManager

+ (MSMobileDeviceManager *)sharedManager
{
    static dispatch_once_t pred = 0;
    __strong static id _sharedObject = nil;
    dispatch_once(&pred, ^{
        _sharedObject = [[self alloc] init]; // or some other init method
    });
    return _sharedObject;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        
        [self initEnvironment];
        [self startListeningDevicesConnection];
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [self stopListeningDeviceConnection];
    if (_waitingInRunLoop) CFRunLoopStop(CFRunLoopGetCurrent());
}

- (void)initEnvironment {
    
    _subscribed = NO;					// we are not subscribed yet
    _waitingInRunLoop = NO;			// we are not currently waiting in a runloop
}

// this is (indirectly) called back by AMDeviceNotificationSubscribe()
// whenever something interesting happens
- (void)Notify:(struct am_device_notification_callback_info*)info
{
    //过滤WIFI设备
    if (AMDeviceGetInterfaceType(info->dev) == 2) {
        
        return;
    }
    
    switch (info->msg) {
        case ADNCI_MSG_UNSUBSCRIBED:
            NSLog(@"Unkown error of USB device connected");
            return;
            
        case ADNCI_MSG_CONNECTED:
        {
        
        am_device device = info->dev;
        uint32_t error = AMDeviceConnect(device);
        if (error == 0) {
            
            error = AMDeviceStartSession(device);
            
            if (error == 0) {
                
                //get device info
                CFTypeRef typeRef = AMDeviceCopyValue(device, NULL, NULL);
                
                id result;
                if (typeRef) {
                    
                    result = (__bridge id)(typeRef);
                }
                
                if (typeRef != NULL) {
                    
                    CFRelease(typeRef);
                }
                
                [self deviceConnectionNotificationHandler:@{@"MessageType":@"Attached", @"Device":result}];
            }
        }
        }
            break;
        case ADNCI_MSG_DISCONNECTED:
        {
        NSString *uniqueId = (__bridge NSString *)(AMDeviceCopyDeviceIdentifier(info->dev));
        
        [self deviceConnectionNotificationHandler:@{@"MessageType":@"Detached", @"UniqueId":uniqueId}];
        }
            break;
            
        default:
#if DEBUG
            NSLog(@"Ignoring unknown message: %d",info->msg);
#endif
            return;
    }
    
    // if he's waiting for us to do something, break him
    if (_waitingInRunLoop) CFRunLoopStop(CFRunLoopGetCurrent());
}

static void notify_callback(struct am_device_notification_callback_info *info, void* arg)
{
    @autoreleasepool {
        
        MSMobileDeviceManager *objSelf = (__bridge MSMobileDeviceManager *)(arg);
        [objSelf Notify:info];
    }
}

- (void)startListeningDevicesConnection {
    
    if (!_subscribed) {
        
        void *objSelf = (__bridge_retained void *)(self);
        
        // try to subscribe for notifications - pass self as the callback_data
        int ret = AMDeviceNotificationSubscribe(notify_callback, 0, 0, objSelf, &_notification);
        if (ret == 0) {
            _subscribed = YES;
        } else {
            // we should throw or something in here...
            NSLog(@"AMDeviceNotificationSubscribe failed: %d", ret);
        }
    }
}

- (void)stopListeningDeviceConnection {
    
    if (_subscribed && _notification) {
        
        uint32_t error = AMDeviceNotificationUnsubscribe(_notification);
        if (error == 0) {
            
            _subscribed = NO;
            _notification = NULL;
        } else {
            
            // we should throw or something in here...
            NSLog(@"AMDeviceNotificationUnsubscribe failed: %d", error);
        }
    }
}

- (void)deviceConnectionNotificationHandler:(NSDictionary *)packet {
    
    NSString *notificationName;
    NSString *messageType = [packet objectForKey:@"MessageType"];
    
    if ([messageType isEqualToString:@"Attached"]) {
        
        notificationName = kMSDeviceConnectedNotification;
    }
    if ([messageType isEqualToString:@"Detached"]) {
        
        notificationName = kMSDeviceDisConnecteNotification;
    }
    
    if (notificationName) {
        
        [[NSNotificationCenter defaultCenter] postNotificationName:notificationName object:self userInfo:packet];
    }
}
@end
