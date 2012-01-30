
/*Copyright (C) 2012 Aaron Skomra and Braden Wooley
 
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

/** 
 *  AppController.m
 *  xtoq
 *
 * 
 *  This is the controller for the Popup to retreive the display number
 *  from the user.

 */

#import "AppController.h"


@implementation AppController


- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
    [[NSGraphicsContext currentContext]
     setImageInterpolation:NSImageInterpolationHigh];
    
    xtoqWindow = [[XtoqWindow alloc] initWithContentRect: NSMakeRect(0, 0, 1028, 768)
                                               styleMask: (NSTitledWindowMask |
                                                           NSMiniaturizableWindowMask |
                                                           NSResizableWindowMask)
                                                 backing: NSBackingStoreBuffered
                                                   defer: YES];
    
    
    // Create and show menu - http://cocoawithlove.com/2010/09/minimalist-cocoa-programming.html
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    id menubar = [NSMenu new];
    id appMenuItem = [NSMenuItem new];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];    
    
    id appMenu = [NSMenu new];
    id appName = [[NSProcessInfo processInfo] processName];
    
    // About
    id aboutTitle = [@"About " stringByAppendingString:appName];        
    id aboutMenuItem = [[NSMenuItem alloc] initWithTitle:aboutTitle action:NULL keyEquivalent:@"a"]; // About is greyed out since action is null
    [appMenu addItem:aboutMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    // Quit    
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    id window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200) styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO];
    [window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
    [window setTitle:appName];
    
    appMenu = [NSMenu new];
    appMenuItem = [NSMenuItem new];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];    
    // [window makeKeyAndOrderFront:nil];
    // [NSApp activateIgnoringOtherApps:YES];

    
    // This was pulled from XqtoqView
    screen = ":1";
    NSLog(@"screen = %s", screen);
    xcbContext = xtoq_init(screen);
    imageT = xtoq_get_image(xcbContext);
    image = [[XtoqImageRep alloc] initWithData:imageT];
    
    ourView = [[XtoqView alloc] initWithImage:image];
    [[xtoqWindow contentView]  addSubview: ourView];

}

- (void) applicationDidFinishLaunching: (NSNotification *) aNotification
{
    [xtoqWindow makeKeyAndOrderFront: self];
    [NSThread detachNewThreadSelector:@selector(wait_for_xtoq_event) toTarget:self withObject:nil];

    
}

- (IBAction)showDisplayChooser
{
   if (!displayNumberController){
        displayNumberController = [[DisplayNumberController alloc] init];
    }
    NSLog(@"showing %@", displayNumberController);
    [displayNumberController showWindow:self];
    NSLog(@"opened %@", displayNumberController);	
}



/**
 *
 * Following the steps from David's email, I produced this which does some
 * weird stuff.
 *
 */
- (void) wait_for_xtoq_event {
    xtoq_context_t xqcontxt;
    xtoq_event_t xqevent;

    while (1) {
        xqevent = dummy_xtoq_wait_for_event(xqcontxt);
    
        if (xqevent.event_type == XTOQ_DAMAGE) {
            NSLog(@"Got damage event");
        }
        else { NSLog(@"HEy I'm Not damage!"); }

    }
}

- (void) putImage {
        NSLog(@"Put Image");
    imageT = xtoq_get_image(xcbContext);
    image = [[XtoqImageRep alloc] initWithData:imageT];

}

@end
