//
//  AppDelegate.m
//  MSMobileDeviceDemo
//
//  Created by MartinLau on 2017/3/19.
//  Copyright © 2017年 Martin Studio. All rights reserved.
//

#import "AppDelegate.h"
#import "MSMobileDeviceManager.h"


@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    [[NSNotificationCenter defaultCenter] addObserverForName:kMSDeviceConnectedNotification object:[MSMobileDeviceManager sharedManager] queue:nil usingBlock:^(NSNotification * _Nonnull note) {
        
        NSLog(@"%@", note.userInfo);
    }];
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
