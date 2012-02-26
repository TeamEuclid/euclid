
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

- (id) init {
    self = [super init];
    if (self) {
        referenceToSelf = self;
    }
    return self;
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
    
    
    
    // setup X connection and get the initial image from the server
    NSLog(@"screen = %s", screen);
    xcbContext = xtoq_init(screen);
    
    NSLog(@"width = %i, height = %i, x = %i, y = %i", xcbContext->width, 
          xcbContext->height, xcbContext->x, xcbContext->y);
    
    winList = [[NSMutableDictionary alloc] init];
    winCount = 0;
    
    [[NSGraphicsContext currentContext]
     setImageInterpolation:NSImageInterpolationHigh];
    
    xtoqWindow = [[XtoqWindow alloc] 
                  initWithContentRect: NSMakeRect(xcbContext->x, xcbContext->y, 
                                                  xcbContext->width, xcbContext->height)
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
    // Start the event loop and set the handler function
	xtoq_start_event_loop(xcbContext, (void *) eventHandler);
}

- (void) keyDownInView: (NSNotification *) aNotification
{
    NSDictionary *keyInfo = [aNotification userInfo];
    // note this keyInfo is the key in <key, value> not the key pressed
    NSEvent * event = [keyInfo objectForKey: @"1"];
    //NSLog(@"Controller Got a XTOQviewKeyDownEvent key %@", [event characters]);
    unsigned short aChar = [[event characters] characterAtIndex:0];
    NSString* charNSString = [event characters]; 
    const char* charcharstar = [charNSString UTF8String];
    printf( "\n--------------------------------------------\n" );
    // translate key here code = translate(charcharstar);
    NSLog(@"%s pressed", charcharstar);
    uint8_t code = (unsigned char)0x10;
    dispatch_async(xtoqDispatchQueue, 
                   ^{ dummy_xtoq_key_press(xcbContext, 
                                     (int)[event windowNumber],
                                     code) ;});
}

- (void) mouseButtonDownInView: (NSNotification *) aNotification
{
    NSDictionary *mouseDownInfo = [aNotification userInfo];
    // NSLog(@"Controller Got a XTOQmouseButtonDownEvent");
    NSEvent * event = [mouseDownInfo objectForKey: @"2"];
    //NSLog(@"Mouse Info: %@", [mouseDownInfo objectForKey: @"2"]);
    dispatch_async(xtoqDispatchQueue, 
                   ^{ dummy_xtoq_button_down (xcbContext,
                                        [event locationInWindow].x, 
                                        [event locationInWindow].y, 
                                        (int)[event windowNumber],
                                        0);;});
}

// create a new image to redraw part of the screen 
- (void) updateImage {

    int numberOfRects = 1;
	int i;
    
    for (i = 0; i < numberOfRects; i++) {
    
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


- (XtoqWindow *) getWindowInList: (xtoq_context_t *)xtoqContxt {
    
    id key;
    int index;
    xtoq_context_t *xqWinContxt;
    NSArray *keyArray = [winList allKeys];
    XtoqWindow *xqWin;
    
    for (index = 0; index < [keyArray count]; index++) {
        key = [keyArray objectAtIndex:index];
        xqWin = [winList objectForKey:key];
        xqWinContxt = [xqWin getContext:xqWin];
        if (xqWinContxt->window == xtoqContxt->window) {
            return xqWin;
        }
    }
    
    return nil;
}


- (void) addWindowInList:(XtoqWindow *)xqWin 
             withContext: (xtoq_context_t *) aContext{
    NSString *key = [NSString stringWithFormat:@"%d", winCount];
    [xqWin setContext:aContext withId:key];
    [winList setObject:xqWin forKey:key];
    ++winCount;
}

- (void) removeWindowInLIst:(id)akey {
    [winList removeObjectForKey:akey];
    --winCount;
}


- (void) setScreen:(const char *)scrn {
    free(screen);
    screen = strdup(scrn);
    NSLog(@"screen = %s", screen);
    if(screen == NULL) {
        perror(strerror(errno));
    }
    else {
        setenv("DISPLAY", screen, 1);
    }
}

- (void) makeMenu {
    // Create and show menu - http://cocoawithlove.com/2010/09/minimalist-cocoa-programming.html
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    // The whole Menu bar in general
    NSMenu *menubar = [NSMenu new];
    NSMenuItem *appMenuItem = [NSMenuItem new];

    // Menu under XtoQ
    NSMenu *appMenu = [NSMenu new];
    NSString *appName = [[NSProcessInfo processInfo] processName];
    
    // Xtoq -> About
    NSString *aboutTitle = [@"About " stringByAppendingString:appName];
    NSMenuItem *aboutMenuItem = [[NSMenuItem alloc] initWithTitle:aboutTitle 
                                                           action:NULL 
                                                    keyEquivalent:@"a"];
    [appMenu addItem:aboutMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    // Xtoq -> Quit    
    NSString *quitTitle = [@"Quit " stringByAppendingString:appName];
    NSMenuItem *quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle 
                                                          action:@selector(terminate:) 
                                                   keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    // Menu under Applications
    NSMenu *startXMenu = [NSMenu new];
    NSMenuItem *startXApps = [NSMenuItem new];
    [startXMenu setTitle:@"Applications"];
    
    // Run Xeyes
    NSString *xTitle;
    NSMenuItem *xeyesMenuItem;
    xTitle = @"Run Xeyes";
    xeyesMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle 
                                               action:@selector(runXeyes:) 
                                        keyEquivalent:@""];
    [startXMenu addItem:xeyesMenuItem];
    [startXApps setSubmenu:startXMenu];
    
    // Run Xclock
    NSMenuItem *xclockMenuItem;
    xTitle = @"Run Xclock";
    xclockMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle 
                                                action:@selector(runXclock:)
                                         keyEquivalent:@""];
    [startXMenu addItem:xclockMenuItem];
    [startXApps setSubmenu:startXMenu];
    
    // Run Xlogo
    NSMenuItem *xlogoMenuItem;
    xTitle = @"Run Xlogo";
    xlogoMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle 
                                               action:@selector(runXlogo:)
                                        keyEquivalent:@""];
    [startXMenu addItem:xlogoMenuItem];
    [startXApps setSubmenu:startXMenu];
    
    // Run Xterm, does not seem to properly launch Xterm, might just scrap this.
    NSMenuItem *xtermMenuItem;
    xTitle = @"Run Xterm";
    xtermMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle 
                                               action:NULL //@selector(runXterm:)
                                        keyEquivalent:@""];
    [startXMenu addItem:xtermMenuItem];
    [startXApps setSubmenu:startXMenu];
    
    // Adding all the menu items to the main menu for XtoQ.
    [menubar addItem:appMenuItem];
    [menubar addItem:startXApps];
    [NSApp setMainMenu:menubar];
}

- (void) launch_client:(NSString *)filename {
    int status;
    pid_t child;
    const char *newargv[6];
    
    newargv[0] = "/bin/sh";
    newargv[1] = "-c";
    newargv[2] = [filename UTF8String];
    newargv[3] = "-display";
    newargv[4] = getenv("DISPLAY");
    newargv[5] = NULL;
    
    status = posix_spawnp(&child, newargv[0], NULL, NULL, (char * const *) newargv, environ);
    if(status) {
        NSLog(@"Error spawning file for launch.");
    }
}

- (void) runXeyes:(id) sender {
    [self launch_client:@"xeyes"];
}
- (void) runXclock:(id) sender {
    [self launch_client:@"xclock"];
}
- (void) runXlogo:(id) sender {
    [self launch_client:@"xlogo"];    
}
- (void) runXterm:(id) sender {
    [self launch_client:@"xterm"];    
}

// create a new window 
- (void) createNewWindow: (xtoq_context_t *) windowContext {
    
    XtoqWindow *newWindow;
    XtoqView *newView;
    xcb_image_t *xcbImage;
    XtoqImageRep *imageRep;
    
    NSLog(@"In createNewWindow");
    NSLog(@"width = %i, height = %i, x = %i, y = %i", windowContext->width, windowContext->height, windowContext->x, windowContext->y);
    
   /* newWindow =  [[XtoqWindow alloc] initWithContentRect: 
                        NSMakeRect(windowContext->x, windowContext->y, windowContext->width, windowContext->height)
                                 styleMask: (NSTitledWindowMask |
                                 NSMiniaturizableWindowMask |
                                 NSResizableWindowMask)
                                 backing: NSBackingStoreBuffered
                                 defer: YES];
    */
    
    newWindow =  [[XtoqWindow alloc] initWithContentRect: 
                  NSMakeRect(100, 150, 1024,768)
                                               styleMask: (NSTitledWindowMask |
                                                           NSMiniaturizableWindowMask |
                                                           NSResizableWindowMask)
                                                 backing: NSBackingStoreBuffered
                                                   defer: YES];

    
    windowContext->local_data = newWindow;
    
    // need to add window to list
    // addWindowInList:(XtoqWindow *)xqWin
    
    
    //create an XtoqImageRep with the information from X
//wrong context 
    xcbImage = xtoq_get_image(windowContext);
    imageRep = [[XtoqImageRep alloc] initWithData:xcbImage];
    
    //draw the image into a rect
    NSRect imgRec = NSMakeRect(0,0, [imageRep getWidth], [imageRep getHeight]);
    
    // create a view, init'ing it with our rect
    newView = [[XtoqView alloc] initWithFrame:imgRec];
    
    //shows the window
    [newWindow makeKeyAndOrderFront:nil];
        
    
    // set the initial image in the window
    [newView setImage:imageRep];
    
    // add view to its window
    [[newWindow contentView]  addSubview: newView]; 
    

//Need to add to the widow list 
 /*   // add root window to list, increment count of windows
    NSString *key = [NSString stringWithFormat:@"%d", winCount];
    [xtoqWindow setContext:xcbContext withId:key];
    [xtoqWindow setRootDataPointer:xcbContext];
    [winList setObject:xtoqWindow forKey:key];
    ++winCount;

*/


}

@end

void eventHandler (xtoq_event_t event)
{
    if (event.event_type == XTOQ_DAMAGE) {
        // This message generates a lot of console spam - only uncomment when testing
        //NSLog(@"Got damage event");
        [referenceToSelf updateImage];
    } else if (event.event_type == XTOQ_CREATE) {
        NSLog(@"Window was created");
        [referenceToSelf createNewWindow: event.context];
    } else if (event.event_type == XTOQ_DESTROY) {
        [referenceToSelf updateImage];
    } else { 
        NSLog(@"Hey I'm Not damage!"); 
    }
    
}
