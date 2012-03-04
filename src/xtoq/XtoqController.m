
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

#define WINDOWBAR 22

@implementation XtoqController

- (id) init {
    self = [super init];
    if (self) {
        referenceToSelf = self;
    }
    return self;
}

- (int) xserverToOSX:(int)yValue windowHeight:(int)windowH {
    
    int height = [[NSScreen mainScreen] frame].size.height;    
    return height - WINDOWBAR - windowH + yValue;
    
}

- (int) osxToXserver:(int)yValue windowHeight:(int)windowH {
    
    int height = [[NSScreen mainScreen] frame].size.height;    
    return height - yValue;
    
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
    
    // setup X connection and get the initial image from the server
    rootContext = xtoq_init(screen);
    
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

	[xtoqWindow setContext: rootContext];
	rootContext->local_data = xtoqWindow;
    // Make the menu
    [self makeMenu];
    
    //create an XtoqImageRep with the information from X
    //libImageT = xtoq_get_image(rootContext);
    //image = [[XtoqImageRep alloc] initWithData:libImageT x:0 y:0];  
    //draw the image into a rect
    imageRec = NSMakeRect(0, 0, 1028,768);//[image getWidth], [image getHeight]);
    // create a view, init'ing it with our rect
    ourView = [[XtoqView alloc] initWithFrame:imageRec];
    // add view to its window
    [xtoqWindow setContentView: ourView];  
    // set the initial image in the window
    //[ourView setImage:image];

    originalWidth = [image getWidth];
    originalHeight = [image getHeight];
    //[ourView setPartialImage:imageNew];
    
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
    unsigned short aChar = [event keyCode];
    NSString* charNSString = [event characters]; 
    const char* charcharstar = [charNSString UTF8String];
    printf( "\n--------------------------------------------\n" );
    // translate key here code = translate(charcharstar);
    NSLog(@"%s pressed", charcharstar);
    //uint8_t code = (unsigned char)0x10;
    //uint8_t code = 
    
	//    for(i = 8; i < 256; i++){
    //    aChar++;
        dispatch_async(xtoqDispatchQueue, 
                   ^{ dummy_xtoq_key_press(rootContext, 
                                     (int)[event windowNumber],
                                     aChar + 8) ;});
		// }
}
 

// on this side all I have is a xtoq_context , on the library side I need
// to turn that into a real context 
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
    
    appMenu = [[NSMenu new] autorelease];
    appName = [[NSProcessInfo processInfo] processName];

    // Xtoq -> About
    aboutTitle = [@"About " stringByAppendingString:appName];
    aboutMenuItem = [[NSMenuItem alloc] initWithTitle:aboutTitle 
                                                           action:NULL 
                                                    keyEquivalent:@"a"];
    [appMenu addItem:aboutMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    // Xtoq -> Quit
    quitTitle = [@"Quit " stringByAppendingString:appName];
    quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle 
                                                          action:@selector(terminate:) 
                                                   keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    // Menu under Applications
    NSMenu *startXMenu = [[NSMenu new] autorelease];
    NSMenuItem *startXApps = [[NSMenuItem new] autorelease];
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
                                               action:@selector(runXterm:)
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
    const char *file_name = [filename UTF8String];
    const char *newargv[4];
    
    asprintf(&newargv[0], "/usr/X11/bin/%s", file_name);
    newargv[1] = "-display";
    newargv[2] = screen;
    newargv[3] = NULL;
    
    status = posix_spawn(&child, newargv[0], NULL, NULL, (char * const *) newargv, environ);
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
    
    XtoqWindow   *newWindow;
    XtoqView     *newView;
    xtoq_image_t *xcbImage;
    XtoqImageRep *imageRep;  

    int y = [self xserverToOSX:windowContext->y windowHeight:windowContext->height];
    
    newWindow =  [[XtoqWindow alloc] 
                  initWithContentRect: NSMakeRect(windowContext->x, y, 
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
    windowContext->local_data = (id)newWindow;
    
    // get image to darw
    xcbImage = xtoq_get_image(windowContext);
    imageRep = [[XtoqImageRep alloc] initWithData:xcbImage x: 0 y: 0];
    
    // draw the image into a rect
    NSRect imgRec = NSMakeRect(0, 0, [imageRep getWidth], [imageRep getHeight]);
    
    // create a view, init'ing it with our rect
    newView = [[XtoqView alloc] initWithFrame:imgRec];
    
    // set the initial image in the window
    [newView setImage:imageRep];
    
    // add view to its window
    [newWindow setContentView: newView ];
    
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
    
    //use dispatch_async() to handle the actual close 
      dispatch_async(xtoqDispatchQueue, ^{
          NSLog(@"Call xtoq_request_close(theContext)");
          xtoq_request_close(theContext);
      });
}

- (void) windowWillMove:(NSNotification*)notification {
    //NSLog(@"window will move");
}
- (void) updateImageNew : (xtoq_context_t *) windowContext
{
    
    float  y_transformed;
	
    libImageT = test_xtoq_get_image(windowContext);
	//	libImageT = xtoq_get_image(windowContext);

    //NSLog(@"update image new values in - %i, %i, %i, %i", windowContext->damaged_x, windowContext->damaged_y, windowContext->damaged_width, windowContext->damaged_height);

    y_transformed =( windowContext->height - windowContext->damaged_y - windowContext->damaged_height)/1.0; 
    imageNew = [[XtoqImageRep alloc] initWithData:libImageT
                                                    x:((windowContext->damaged_x))
                                                    y:y_transformed];
	XtoqView *localView = (XtoqView *)[(XtoqWindow *)windowContext->local_data contentView];
    [ localView setPartialImage:imageNew];
}

- (void) windowDidMove:(NSNotification*)notification {
    [self reshape];
}

- (void) windowDidResize:(NSNotification*)notification {
    [self reshape];
}

- (void) reshape {
    
    XtoqWindow *moveWindow = [NSApp mainWindow];
    
    if (moveWindow != nil) {        
        xtoq_context_t *moveContext = [moveWindow getContext];        
        NSRect moveFrame = [moveWindow frame];
        
        int x = (int)moveFrame.origin.x;
        int y = [self osxToXserver:(int)moveFrame.origin.y windowHeight:moveContext->height];
        int width = (int)moveFrame.size.width;
        int height = (int)moveFrame.size.height;
        NSLog(@"x = %i, y = %i, width = %i, height = %i,", x, y, width, height); 
        NSLog(@"Call xtoq_configure_window(moveContext, x, y, height, width)"); 
        xtoq_configure_window(moveContext, x, y, height, width);       
    }    
}

@end

void eventHandler (xtoq_event_t *event)
{
    xtoq_context_t *context = event->context;
    if (event->event_type == XTOQ_DAMAGE) {
        // This message generates a lot of console spam - only uncomment when testing
        //NSLog(@"Got damage event");
	  [referenceToSelf updateImageNew: context];
    } else if (event->event_type == XTOQ_CREATE) {
        NSLog(@"Window was created");
        [referenceToSelf createNewWindow: context];
    } else if (event->event_type == XTOQ_DESTROY) {
        NSLog(@"Window was destroyed");
        [referenceToSelf destroyWindow: context];
    } else { 
        NSLog(@"Hey I'm Not damage!"); 
    }
    free(event);
}
