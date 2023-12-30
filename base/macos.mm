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

string acorn::ns2str(NSString* s) {
    auto sz = [s cStringUsingEncoding:NSUTF8StringEncoding];
    return string(sz, (uint32_t)strlen(sz));
}
NSString* acorn::str2ns(const string& s) {
    return [NSString stringWithCString:s.c_str() encoding:NSUTF8StringEncoding];
}

bool resolvePath(string& path) {
    if (!path.hasPrefix("//"_S)) {
        return true;
    }
    
    //assets/...
    if (path.hasPrefix("//assets/"_S)) {
        path.erase(0, 1); // remove just the leading slash, leaving the path as '/assets/...'
        string bundlepath = acorn::ns2str([NSBundle bundleForClass:NSClassFromString(@"NativeView")].bundlePath);
#if TARGET_OS_OSX
        bundlepath.append("/Contents/Resources/"_S);
#endif
        path.prepend(bundlepath);
        return true;
    }
    
    //data/... : ~/Library/Application Support/<bundleid>/...
    if (path.hadPrefix("//data/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] lastObject];
        url = [url URLByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
        string fsr(url.fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }
    
    //cache/... : ~/Library/Caches/<bundleid>/...
    if (path.hadPrefix("//cache/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSCachesDirectory inDomains:NSUserDomainMask] lastObject];
        url = [url URLByAppendingPathComponent:[[NSBundle mainBundle] bundleIdentifier]];
        string fsr(url.fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }
    
    //docs/... : ~/Documents/...
    if (path.hadPrefix("//docs/"_S)) {
        NSURL* url = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
        string fsr(url.fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }

    //tmp/... : ~/Documents/...
    if (path.hadPrefix("//tmp/"_S)) {
        string fsr(NSTemporaryDirectory().fileSystemRepresentation, -1);
        fsr.append("/");
        path.prepend(fsr);
        return true;
    }

    // :-(
    log_warn("Unknown path: %s", path.c_str());
    return false;
}
    
