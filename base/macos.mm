//
//  acorn
//
//  Created by Reuben on 07/11/2023.
//

#include "../libacorn.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>


@end

@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    acorn::main();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}


@end

int main(int argc, const char * argv[]) {
    
    NSApplication * application = [NSApplication sharedApplication];
    AppDelegate* del = [AppDelegate new];
    [application setDelegate:del];
    [application run];
    return EXIT_SUCCESS;
}

