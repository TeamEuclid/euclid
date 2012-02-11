
/*Copyright (C) 2012 Aaron Skomra, Ben Huddle, Braden Wooley
 
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
    
    
    //Setting environment variable $DISPLAY to screen.
    char *env = getenv("DISPLAY");
    NSLog(@"$DISPLAY = %s", env);
    if (setenv("DISPLAY", screen, 1) == 0) {
        NSLog(@"setenv successful");
        env = getenv("DISPLAY");
        NSLog(@"current $DISPLAY is = %s", env);
    }
    else {
        NSLog(@"not successful in attemp to set $DISPLAY");
    }
    
    
    // setup X connection and get the initial image from the server
    NSLog(@"screen = %s", screen);
    xcbContext = xtoq_init(screen);
    
NSLog(@"width = %i, height = %i, x = %i, y = %i", xcbContext.width, xcbContext.height, xcbContext.x, xcbContext.y);
    
    winList = [[NSMutableDictionary alloc] init];
    winCount = 0;
    
    [[NSGraphicsContext currentContext]
     setImageInterpolation:NSImageInterpolationHigh];
    
    xtoqWindow = [[XtoqWindow alloc] initWithContentRect: NSMakeRect(xcbContext.x, xcbContext.y, xcbContext.width, xcbContext.height)
                                               styleMask: (NSTitledWindowMask |
                                                           NSMiniaturizableWindowMask |
                                                           NSResizableWindowMask)
                                                 backing: NSBackingStoreBuffered
                                                   defer: YES];

    // Make the menu
    [self makeMenu];        
    
    //create an XtoqImageRep with the information from X
    imageT = xtoq_get_image(xcbContext);
    image = [[XtoqImageRep alloc] initWithData:imageT];  
    //draw the image into a rect
    NSRect imageRec = NSMakeRect(0, 0, [image getWidth], [image getHeight]);
    // create a view, init'ing it with our rect
    ourView = [[XtoqView alloc] initWithFrame:imageRec];
    // add view to its window
    [[xtoqWindow contentView]  addSubview: ourView];  
    // set the initial image in the window
    [ourView setImage:image];
    originalWidth = [image getWidth];
    originalHeight = [image getHeight];
    
    // add root window to list, increment count of windows
    NSString *key = [NSString stringWithFormat:@"%d", winCount];
    [xtoqWindow setContext:xcbContext withId:key];
    [xtoqWindow setRootDataPointer:xcbContext];
    [winList setObject:xtoqWindow forKey:key];
    ++winCount;
    
    // Register for the key down notifications from the view
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(keyDownInView:)
                                                 name: @"XTOQviewKeyDownEvent"
                                               object: nil];
    
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(mouseButtonDownInView:)
                                                 name: @"XTOQmouseButtonDownEvent"
                                               object: nil];
    
    xtoqDispatchQueue = dispatch_queue_create("xtoq.dispatch.queue", NULL);
    
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
    NSEvent * event = [keyInfo objectForKey: @"1"];
    NSLog(@"Controller Got a XTOQviewKeyDownEvent key %@", [event characters]);
    dispatch_async(xtoqDispatchQueue, ^{ xtoq_key_press((int)[event windowNumber], [event keyCode]) ;});
}

- (void) mouseButtonDownInView: (NSNotification *) aNotification
{
    NSDictionary *keyInfo = [aNotification userInfo];
    NSLog(@"Controller Got a XTOQmouseButtonDownEvent");
    // this keyInfo is the key in <key, value>
    NSLog(@"Mouse Info: %@", [keyInfo objectForKey: @"2"]);
    dispatch_async(xtoqDispatchQueue, ^{ NSLog(@"send mouseButton to dispatch");});
    //    dispatch_async(xtoqDispatchQueue, ^{ NSLog(@"send mouseButton to dispatch");});
}


- (void) wait_for_xtoq_event {
    xtoq_context_t xqcontxt;
    xtoq_event_t xqevent;
    
    while (1) {
        // Change this call to dummy_xtoq_wait_for_event if
        // you just want the 4 second delay
        //xqevent = dummy_xtoq_wait_for_event(xcbContext);
        xqevent = xtoq_wait_for_event(xcbContext);    
        
        if (xqevent.event_type == XTOQ_DAMAGE) {
            //NSLog(@"Got damage event");
            [self updateImage];
        } else if (xqevent.event_type == XTOQ_CREATE) {
            [self updateImage];
        } else if (xqevent.event_type == XTOQ_DESTROY) {
            [self updateImage];
        } else { 
            NSLog(@"Hey I'm Not damage!"); 
        }

    }
}

// create a new image to redraw part of the screen 
- (void) updateImage {

    int numberOfRects = 1;
    
    for (int i = 0; i < numberOfRects; i++) {
    
        //NSLog(@"update Image");
        
        xcb_image_destroy(imageT);
        imageT = xtoq_get_image(xcbContext);
        image = [[XtoqImageRep alloc] initWithData:imageT];

        [image topCrop];
        [ourView setPartialImage:image];
        
        //NSRect rect = NSMakeRect(0, 0, [image getWidth]-30, [image getHeight]-30);
         NSRect rect = NSMakeRect(0, 0, originalWidth-30, originalHeight-30);
        [ourView setNeedsDisplayInRect:rect];
    }
}


- (XtoqWindow *) getWindowInList: (xtoq_context_t)xtoqContxt {
    
    id key;
    int index;
    xtoq_context_t xqWinContxt;
    NSArray *keyArray = [winList allKeys];
    XtoqWindow *xqWin;
    
    for (index = 0; index < [keyArray count]; index++) {
        key = [keyArray objectAtIndex:index];
        xqWin = [winList objectForKey:key];
        xqWinContxt = [xqWin getContext:xqWin];
        if (xqWinContxt.window == xtoqContxt.window) {
            return xqWin;
        }
    }
    
    return nil;
}


- (void) addWindowInList:(XtoqWindow *)xqWin 
             withContext: (xtoq_context_t) aContext{
    NSString *key = [NSString stringWithFormat:@"%d", winCount];
    [xqWin setContext:aContext withId:key];
    [winList setObject:xqWin forKey:key];
    ++winCount;
}

- (void) removeWindowInLIst:(id)akey {
    [winList removeObjectForKey:akey];
    --winCount;
}


- (void) setScreen:(char *)scrn {
    screen = scrn;
}

- (void) makeMenu {
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
}

@end
