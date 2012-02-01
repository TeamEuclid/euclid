
/*Copyright (C) 2012 Aaron Skomra, Ben Huddle
 
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

#import "XtoqController.h"


@implementation XtoqController


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
    
    // Leaving these in for testing
    file = @"Xtoq.app/Contents/Resources/Mac-Logo.jpg";
    image2 = [[NSImage alloc] initWithContentsOfFile:(file)];
    
    // Register for the key down notifications from the view
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(keyDownInView:)
                                                 name: @"viewKeyDownEvent"
                                               object: nil];

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

- (void) keyDownInView: (NSNotification *) aNotification
{
    NSDictionary *keyInfo = [aNotification userInfo];
    NSLog(@"Got a viewKeyDownEvent");
    NSLog(@"Key was: %@", [keyInfo objectForKey: @"1"]);
}

- (void) wait_for_xtoq_event {
    xtoq_context_t xqcontxt;
    xtoq_event_t xqevent;
    
    while (1) {
        xqevent = dummy_xtoq_wait_for_event(xqcontxt);
    
        if (xqevent.event_type == XTOQ_DAMAGE) {
            NSLog(@"Got damage event");
            [self updateImage];
        }
        else { 
            NSLog(@"HEy I'm Not damage!"); 
            [self updateImage];
        }

    }
}

- (void) updateImage {
        NSLog(@"update Image");
    imageT = xtoq_get_image(xcbContext);
    image = [[XtoqImageRep alloc] initWithData:imageT];
    [ourView setImage:image];
}

@end
