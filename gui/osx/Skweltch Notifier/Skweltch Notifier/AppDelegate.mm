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

// This function returns a parsed object that you have the responsibility to release/autorelease (see 'create rule' in apple docs)
-(NSString *) createUnpackedObject:(msgpack_object)obj withString:(NSString *)str {
    
    switch (obj.type) {
        case MSGPACK_OBJECT_BOOLEAN:
            if (obj.via.boolean) {
                return [str stringByAppendingString:@"true"];
            }
            else {
                return [str stringByAppendingString:@"false"];
            }
            break;
        case MSGPACK_OBJECT_POSITIVE_INTEGER:
            return [str stringByAppendingString:[NSString stringWithFormat:@"%ld", (long)obj.via.u64]];
            break;
        case MSGPACK_OBJECT_NEGATIVE_INTEGER:
            return [str stringByAppendingString:[NSString stringWithFormat:@"%ld", (long)obj.via.i64]];
            break;
        case MSGPACK_OBJECT_DOUBLE:
            return [str stringByAppendingString:[NSString stringWithFormat:@"%e", obj.via.dec]];
            break;
        case MSGPACK_OBJECT_RAW:
            return [str stringByAppendingString:[[NSString alloc] initWithBytes:obj.via.raw.ptr length:obj.via.raw.size encoding:NSUTF8StringEncoding]];
            break;
        case MSGPACK_OBJECT_ARRAY:
        {
            msgpack_object* const pend = obj.via.array.ptr + obj.via.array.size;
            NSString *newstr = [str copy];
            for(msgpack_object *p= obj.via.array.ptr;p < pend;p++) {
                newstr = [self createUnpackedObject:*p withString:newstr];
            }
            return newstr;
        }
            break;
        case MSGPACK_OBJECT_MAP:
        case MSGPACK_OBJECT_NIL:
        default:
            return nullptr;
    }
}

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
        
        NSString *string = [self createUnpackedObject:obj withString:@""];
        
        NSUserNotification *notification = [[NSUserNotification alloc] init];
        [notification setTitle:string];
        [notification setInformativeText:@"Message from ZMQ"];
        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
        
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
