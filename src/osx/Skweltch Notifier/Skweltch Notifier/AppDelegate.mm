//
//  AppDelegate.m
//  Skweltch Notifier
//
//  Created by Paul Hamilton on 15/09/13.
//  Copyright (c) 2013 Paul Hamilton. All rights reserved.
//

#import "AppDelegate.h"

#include <zmq.hpp>
#include <czmq.h>
#include <msgpack.h>

@implementation AppDelegate

- (void)dealloc
{
    [super dealloc];
}

@synthesize portField = _portField;

- (void)listeningToZmq:(id)sender
{
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    
    if (self.shouldBind) {
        receiver.bind(self.tcpUrl.UTF8String);
    }
    else {
        receiver.connect(self.tcpUrl.UTF8String);
    }
    
    msgpack_zone mempool;
    msgpack_zone_init(&mempool, 2048);
    
 	while (1) {
        
		zmq::message_t message;
		try {
			receiver.recv(&message);
		}
		catch (zmq::error_t &e) {
            
		}
        
        msgpack_object obj;
        msgpack_unpack((const char *)message.data(), message.size(), NULL, &mempool, &obj);
        
        if (obj.type == MSGPACK_OBJECT_RAW) {
            
            NSData *data=[[NSMutableData alloc] init];
            [data appendBytes:obj.via.raw.ptr length:obj.via.raw.size];
            NSString *string = [[NSString alloc] initWithData:data
                        encoding:NSASCIIStringEncoding];
            
            NSUserNotification *notification = [[NSUserNotification alloc] init];
            [notification setTitle:string];
            [notification setInformativeText:@"Message from ZMQ"];
            [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
                       
        }
	}
 
	msgpack_zone_destroy(&mempool);

}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:self];
    
    // need to get this
    _tcpUrl = @"tcp://*:5559";
    _shouldBind = true;
    
    if (_shouldBind) {
        [self.portField setStringValue: [NSString stringWithFormat:@"(bound to %@)", _tcpUrl]];
    }
    else {
        [self.portField setStringValue: [NSString stringWithFormat:@"(connected to %@)", _tcpUrl]];
    }
    
    // Create a local port for receiving responses.
    NSThread* myThread = [[NSThread alloc] initWithTarget:self
                                                 selector:@selector(listeningToZmq:)
                                                   object:self];
    [myThread start];  // Actually create the thread
}

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification{
    return YES;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    return NSTerminateNow;
}

@end
