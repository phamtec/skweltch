
#import <Cocoa/Cocoa.h>

#import "Notify.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:self];
}

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification{
    return YES;
}

@end

void notify() {

	NSUserNotification *notification = [[NSUserNotification alloc] init];
	[notification setTitle:@"Hello World"];
	[notification setInformativeText:@"Hello world message"];
	[[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];

}
