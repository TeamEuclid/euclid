/*Copyright (C) 2012 Braden Wooley, Ben Huddle
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#import <AppKit/AppKit.h>
#import "XtoqController.h"
#import "spawn.h"
#import "crt_externs.h"
#import "sys/times.h"
#import "sys/stat.h"
#import "string.h"

#ifdef __APPLE__
#define environ (*_NSGetEnviron())
#endif

@interface XtoqApplication : NSApplication {
    NSNotificationCenter *notificationCenter;
}
/**
 * The main of our Application.
 *
 * Specifically this function starts up Xorg connection, defaulting to 
 * ':0' if it is available.  It also sets the environment variable to the 
 * correct display.
 *
 * @param argc Number of command line arguments.
 * @param argv The command line entered in.
 *
 * @return Returns 1.
 */
int XtoqApplicationMain(int argc, char** argv);

- (void) sendEvent:(NSEvent *)e;

- (int) 
eventToButtonInt:(NSEvent *)e;
@end



