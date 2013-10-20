//
//  AppDelegate.h
//  Skweltch Notifier
//
//  Created by Paul Hamilton on 15/09/13.
//  Copyright (c) 2013 Paul Hamilton. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSUserNotificationCenterDelegate>

@property (assign) IBOutlet NSWindow *window;

@property (assign) NSString* tcpUrl;
@property (assign) Boolean shouldBind;

@property (assign) IBOutlet NSTextField *portField;

@end
