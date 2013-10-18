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

@property (readonly, strong, nonatomic) NSPersistentStoreCoordinator *persistentStoreCoordinator;
@property (readonly, strong, nonatomic) NSManagedObjectModel *managedObjectModel;
@property (readonly, strong, nonatomic) NSManagedObjectContext *managedObjectContext;

- (IBAction)start:(id)sender;
- (IBAction)chooseBind:(id)sender;
- (IBAction)chooseConnect:(id)sender;
- (IBAction)portValue:(id)sender;

@end
