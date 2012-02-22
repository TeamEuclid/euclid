
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
    
    //screen = ":1";// comment out
    // setup X connection and get the initial image from the server
//NSLog(@"screen = %s", screen);
    rootContext = xtoq_init(screen);
    
//NSLog(@"width = %i, height = %i, x = %i, y = %i", rootContext->width, 
//      rootContext->height, rootContext->x, rootContext->y);
    
    
    [[NSGraphicsContext currentContext]
     setImageInterpolation:NSImageInterpolationHigh];
    
    xtoqWindow = [[XtoqWindow alloc] 
                  initWithContentRect: NSMakeRect(rootContext->x, rootContext->y, 
                                                  rootContext->width, rootContext->height)
                            styleMask: (NSTitledWindowMask |
                                        NSClosableWindowMask |
                                        NSMiniaturizableWindowMask |
                                        NSResizableWindowMask)
                              backing: NSBackingStoreBuffered
                                defer: YES];

    // Make the menu
    [self makeMenu];
    
    //create an XtoqImageRep with the information from X
    imageT = xtoq_get_image(xcbContext);
    image = [[XtoqImageRep alloc] initWithData:imageT x:0 y:0];  
    //draw the image into a rect
    imageRec = NSMakeRect(0, 0, [image getWidth], [image getHeight]);
    // create a view, init'ing it with our rect
    ourView = [[XtoqView alloc] initWithFrame:imageRec];
    // add view to its window
    [[xtoqWindow contentView]  addSubview: ourView];  
    // set the initial image in the window
    //[ourView setImage:image];

    originalWidth = [image getWidth];
    originalHeight = [image getHeight];
    [ourView setPartialImage:imageNew x:0 y:0 dx:originalWidth dy:originalHeight];

    //set context
    [xtoqWindow setContext:rootContext];
  
    
    NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
    
    
    // Register for the key down notifications from the view
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(keyDownInView:)
                                                 name: @"XTOQviewKeyDownEvent"
                                               object: nil];
    
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(mouseButtonDownInView:)
                                                 name: @"XTOQmouseButtonDownEvent"
                                               object: nil];
    // register for destroy event
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(destroy:)
                                                 name: @"XTOQdestroyTheWindow"
                                               object: nil];
    
    // regester for window will/did movement notification
    [[NSNotificationCenter defaultCenter] addObserver:self 
                                             selector:@selector(windowWillMove:) 
                                                 name:NSWindowWillMoveNotification 
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self 
                                             selector:@selector(windowDidMove:) 
                                                 name:NSWindowDidMoveNotification 
                                               object:nil];

    // regester for window resize notification
    [[NSNotificationCenter defaultCenter] addObserver:self 
                                             selector:@selector(windowDidResize:) 
                                                 name:NSWindowDidResizeNotification 
                                               object:nil];
    
    xtoqDispatchQueue = dispatch_queue_create("xtoq.dispatch.queue", NULL);
    
}

- (void) applicationDidFinishLaunching: (NSNotification *) aNotification
{
    [xtoqWindow makeKeyAndOrderFront: self];
    
    //hide window
    [xtoqWindow orderOut:self];
    
    // Start the event loop and set the handler function
	xtoq_start_event_loop(rootContext, (void *) eventHandler);
}

- (void) keyDownInView: (NSNotification *) aNotification
{   
    int i = 0;
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
    //uint8_t code = (unsigned char)0x10;
    //uint8_t code = 
    
    for(i = 0; i < 256; i++){
        aChar++;
        dispatch_async(xtoqDispatchQueue, 
                   ^{ dummy_xtoq_key_press(xcbContext, 
                                     (int)[event windowNumber],
                                     aChar) ;});
    }
}

- (void) mouseButtonDownInView: (NSNotification *) aNotification
{
    CGFloat heightFloat;
    NSDictionary *mouseDownInfo = [aNotification userInfo];
    // NSLog(@"Controller Got a XTOQmouseButtonDownEvent");
    NSEvent * event = [mouseDownInfo objectForKey: @"1"];
    //NSRect bnd = NSMakeRect(0,0,512,386);
    NSNumber * heightAsNumber =  [NSNumber alloc];
    heightAsNumber = [mouseDownInfo objectForKey: @"2"];
    heightFloat = [heightAsNumber floatValue];
    //NSLog(@"Mouse Info: %@", [mouseDownInfo objectForKey: @"2"]);
    dispatch_async(xtoqDispatchQueue, 
                   ^{ dummy_xtoq_button_down (rootContext,
                                        [event locationInWindow].x, 
                                        heightFloat - [event locationInWindow].y, 
                                        (int)[event windowNumber],
                                        0);;});
}


    int numberOfRects = 1;
	int i;
    
    for (i = 0; i < numberOfRects; i++) {
    
        //NSLog(@"update Image");
        
        xcb_image_destroy(imageT);
        imageT = xtoq_get_image(rootContext);
        image = [[XtoqImageRep alloc] initWithData:imageT];

        [image topCrop];
        [ourView setPartialImage:image x:0 y:0];
        
        //NSRect rect = NSMakeRect(0, 0, [image getWidth]-30, [image getHeight]-30);
         NSRect rect = NSMakeRect(0, 0, originalWidth-30, originalHeight-30);
        [ourView setNeedsDisplayInRect:rect];
    }
}


- (void) setScreen:(char *)scrn {
    screen = scrn;
}

- (void) makeMenu {
    // Create and show menu - http://cocoawithlove.com/2010/09/minimalist-cocoa-programming.html    
    NSMenu *menubar;
    NSMenuItem *appMenuItem;
    NSMenu *appMenu;
    NSString *appName;
    NSString *aboutTitle;
    NSMenuItem *aboutMenuItem;
    NSString *quitTitle;
    NSMenuItem *quitMenuItem;

    menubar = [[NSMenu new] autorelease];
    appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];    
    
    appMenu = [[NSMenu new] autorelease];
    appName = [[NSProcessInfo processInfo] processName];
    
    // About
    aboutTitle = [@"About " stringByAppendingString:appName];        
    aboutMenuItem = [[NSMenuItem alloc] initWithTitle:aboutTitle
										action:NULL
										keyEquivalent:@"a"]; // About is greyed out since action is null
    [appMenu addItem:aboutMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    // Quit    
    quitTitle = [@"Quit " stringByAppendingString:appName];
    quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
									   action:@selector(terminate:)
									   keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    
    
    
    //TEST
    // Menu under Applications
    NSMenu *startXMenu = [NSMenu new];
    NSMenuItem *startXApps = [NSMenuItem new];
    [startXMenu setTitle:@"Applications"];
    
    // Run Xeyes
    NSString *xTitle;
    NSMenuItem *xeyesMenuItem;
    xTitle = [@"Run Xeyes on " stringByAppendingString:appName];
    xeyesMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle action:@selector(runXeyes:) keyEquivalent:@"e"];
    [startXMenu addItem:xeyesMenuItem];
    [startXApps setSubmenu:startXMenu];
    
    // Run Xclock
    NSMenuItem *xclockMenuItem;
    xTitle = [@"Run Xclock on" stringByAppendingString:appName];
    xclockMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle action:NULL keyEquivalent:@"c"];
    [startXMenu addItem:xclockMenuItem];
    [startXApps setSubmenu:startXMenu];
    // Run Xlogo
    NSMenuItem *xlogoMenuItem;
    xTitle = [@"Run Xlogo on" stringByAppendingString:appName];
    xlogoMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle action:NULL keyEquivalent:@"l"];
    [startXMenu addItem:xlogoMenuItem];
    [startXApps setSubmenu:startXMenu];
    // Run Xterm
    NSMenuItem *xtermMenuItem;
    xTitle = [@"Run Xterm on" stringByAppendingString:appName];
    xtermMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle action:NULL keyEquivalent:@"t"];
    [startXMenu addItem:xtermMenuItem];
    [startXApps setSubmenu:startXMenu];
    // Run Xman
    NSMenuItem *xmanMenuItem;
    xTitle = [@"Run Xman on" stringByAppendingString:appName];
    xmanMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle action:NULL keyEquivalent:@"m"];
    [startXMenu addItem:xmanMenuItem];
    [startXApps setSubmenu:startXMenu];
    
    //idea: might just have one "run" function and send the string of the
    // executable to sprintf'd onto the end of the path.
    
    //TEST
    
    
    
    [menubar addItem:appMenuItem];
    [menubar addItem:startXApps];
    [NSApp setMainMenu:menubar];
}

- (void) runXeyes:(id) sender {
    
    NSLog(@"Attempting to run Xeyes.");
    const char *xArg[4];
    xArg[0] = "/usr/X11/bin/xeyes";
    xArg[1] = "-display";
    xArg[2] = getenv("DISPLAY");
    xArg[3] = NULL;
    
    // might try double fork like Xquartz
    
}
- (void) runXclock:(id) sender {
}
- (void) runXlogo:(id) sender {
}
- (void) runXterm:(id) sender {
}
- (void) runXman:(id) sender {
}

// create a new window 
- (void) createNewWindow: (xtoq_context_t *) windowContext {
    
    XtoqWindow *newWindow;
    XtoqView *newView;
    xcb_image_t *xcbImage;
    XtoqImageRep *imageRep;
    //NSLog(@"width = %i, height = %i, x = %i, y = %i", windowContext->width, windowContext->height, windowContext->x, windowContext->y);    
    
    newWindow =  [[XtoqWindow alloc] 
                  initWithContentRect: NSMakeRect(windowContext->x, windowContext->y, 
                                                  windowContext->width, windowContext->height)
                  styleMask: (NSTitledWindowMask |
                              NSClosableWindowMask |
                              NSMiniaturizableWindowMask |
                              NSResizableWindowMask)
                  backing: NSBackingStoreBuffered
                  defer: YES];
    
    // save context in window
    [newWindow setContext:windowContext];
    
    // save the newWindow pointer into the context
    windowContext->local_data = newWindow;
    
    // get image to darw
    xcbImage = xtoq_get_image(windowContext);
    imageRep = [[XtoqImageRep alloc] initWithData:xcbImage];
    
    // draw the image into a rect
    NSRect imgRec = NSMakeRect(0,0, [imageRep getWidth], [imageRep getHeight]);
    
    // create a view, init'ing it with our rect
    newView = [[XtoqView alloc] initWithFrame:imgRec];
    
    // set the initial image in the window
    [newView setImage:imageRep];
    
    // add view to its window
    [[newWindow contentView]  addSubview: newView]; 
    
    // set title
    NSString *winTitle;
    winTitle = [NSString stringWithCString:windowContext->name encoding:NSUTF8StringEncoding];
    [newWindow setTitle:winTitle];
    
    //shows the window
    [newWindow makeKeyAndOrderFront:self];
    
}

- (void) destroyWindow:(xtoq_context_t *) windowContext {
    // set the window to be closed
    XtoqWindow *destWindow = windowContext->local_data;
    //close window
    [destWindow close];
}

- (void) destroy:(NSNotification *) aNotification {    
    
    NSDictionary *contextInfo = [aNotification userInfo];    
    XtoqWindow *aWindow = [contextInfo objectForKey: @"1"];
    xtoq_context_t *theContext = [aWindow getContext];
/*   
    NSLog(@"windowContext");
    NSLog(@"x = %i",theContext->x);
    NSLog(@"y = %i",theContext->y);
    NSLog(@"width = %i",theContext->width);
    NSLog(@"height = %i",theContext->height);
*/   
    
    //use dispatch_async() to handle the actual close 
      dispatch_async(xtoqDispatchQueue, ^{
          NSLog(@"Call xtoq_close_window(theContext)");
          //xtoq_close_window(theContext);
      });
}

- (void)windowWillMove:(NSNotification*)notification
{
    NSLog(@"window will move");
}

- (void)windowDidMove:(NSNotification*)notification
{
    NSLog(@"window did move");    
    
    XtoqWindow *moveWindow = [NSApp mainWindow];
    
    if (moveWindow != nil) {
        // origin is the NSPoint that is part of the frame  
        // x & y are CGFloat
        // (0,0) is the bottom left of the screen
        xtoq_context_t *moveContext = [moveWindow getContext];        
        NSRect moveFrame = [moveWindow frame];
        
        // update context
        moveContext->x = (int)moveFrame.origin.x;
        moveContext->y = (int)moveFrame.origin.y;       
        
        NSLog(@"Window title \"%s\" ",moveContext->name);
        NSLog(@"window context after x = %d, y = %d ", moveContext->x, moveContext->y);
        NSLog(@"Call xtoq_updatewindowposition(moveContext)"); 
        //xtoq_updatewindowposition (moveContext);       
    }
}

- (void)windowDidResize:(NSNotification*)notification {
    XtoqWindow *resizeWindow = [NSApp mainWindow];
    if (resizeWindow != nil) {
        NSLog(@"Window did resize, change the size");
    }
}

@end
- (void) updateImageNew : (xtoq_context_t *) windowContext{
    float x_transformed, y_transformed, dWidth,dHeight;
    int numberOfRects = 1;
	int i;
        
        if (libImageT.image)
            xcb_image_destroy(libImageT.image);
        libImageT = test_xtoq_get_image(*windowContext);
        NSLog(@"update image new values in - %i, %i, %i, %i", windowContext->damaged_x, windowContext->damaged_y, windowContext->damaged_width, windowContext->damaged_height);

        y_transformed =( windowContext->height - windowContext->damaged_y - windowContext->damaged_height)/1.0; 
        imageNew = [[XtoqImageRep alloc] initWithData:libImageT.image
                                                    x:(windowContext->damaged_x)/1.0
                                                    y:y_transformed];
        [ourView setPartialImage:imageNew];

}

void eventHandler (xtoq_event_t *event)
{
    if (event->event_type == XTOQ_DAMAGE) {
        // This message generates a lot of console spam - only uncomment when testing
        //NSLog(@"Got damage event");
        [referenceToSelf updateImage];
    } else if (event->event_type == XTOQ_CREATE) {
        NSLog(@"Window was created");
        [referenceToSelf createNewWindow: event->context];
    } else if (event->event_type == XTOQ_DESTROY) {
        NSLog(@"Window was destroyed");
        [referenceToSelf destroyWindow: event->context];
    } else { 
        NSLog(@"Hey I'm Not damage!"); 
    }
    free(event);
}
