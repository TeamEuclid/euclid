
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
    return height - windowH + yValue;
    
}

- (int) osxToXserver:(int)yValue windowHeight:(int)windowH {
    
    int height = [[NSScreen mainScreen] frame].size.height;    
    return height - yValue;
    
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
  
    // setup X connection and get the initial image from the server
    rootContext = xtoq_init(screen);
    
    [[NSGraphicsContext currentContext]
    setImageInterpolation:NSImageInterpolationHigh];
  
	// TODO: We create a window for the "root", but currently it is
	// never displayed. Need to think about if this needs to be
	// reworked for a "rooted" mode or if this code should simply be
	// removed.
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
	[ourView setContext: rootContext];

    // add view to its window
    [xtoqWindow setContentView: ourView];  
    // set the initial image in the window
    //[ourView setImage:image];

   // originalWidth = [image getWidth];
   // originalHeight = [image getHeight];
    //[ourView setPartialImage:imageNew];
    
    NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
    
    
    // Register for the key down notifications from the view
    [nc addObserver: self
		   selector: @selector(keyDownInView:)
		       name: @"XTOQviewKeyDownEvent"
		     object: nil];
    
    [nc addObserver: self
		   selector: @selector(keyUpInView:)
		       name: @"XTOQviewKeyUpEvent"
		     object: nil];
    
    [nc addObserver: self
		   selector: @selector(mouseButtonDownInView:)
		       name: @"XTOQmouseButtonDownEvent"
		     object: nil];
    
    [nc addObserver: self
		   selector: @selector(mouseButtonReleaseInView:)
		       name: @"XTOQmouseButtonReleaseEvent" 
		     object: nil];

    [nc addObserver: self
           selector: @selector(mouseMovedInApp:)
               name: @"MouseMovedEvent" 
             object: nil];

    // register for destroy event
    [nc addObserver: self
		   selector: @selector(destroy:)
		       name: @"XTOQdestroyTheWindow"
		     object: nil];
    
    // regester for window will/did movement notification
    [nc addObserver:self 
		   selector:@selector(windowWillMove:) 
		       name:NSWindowWillMoveNotification 
		     object:nil];
    
    [nc addObserver:self 
		   selector:@selector(windowDidMove:) 
		       name:NSWindowDidMoveNotification 
		     object:nil];

    // regester for window resize notification
    [nc addObserver:self 
		   selector:@selector(windowDidResize:) 
		       name:NSWindowDidResizeNotification 
		     object:nil];
   /* [nc addObserver:self
	         selector:@selector(applicationWillTerminate:)
			     name:NSApplicationWillTerminateNotification object:nil]; */
    
    

    xtoqDispatchQueue = dispatch_queue_create("xtoq.dispatch.queue", NULL);
    
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    xtoq_close();
    
    const char *spawn[4];
    pid_t child;
     
    spawn[0] = "/usr/bin/killall";
    spawn[1] = "-9";
    spawn[2] = "Xorg";
    spawn[3] = NULL;
     
    posix_spawn(&child, spawn[0], NULL, NULL, (char * const*)spawn, environ);
}

- (void) applicationDidFinishLaunching: (NSNotification *) aNotification
{
    // Start the event loop and set the handler function
	xtoq_start_event_loop(rootContext, (void *) eventHandler);
}

- (void) mouseMovedInApp: (NSNotification *) aNotification {

    //CGFloat heightFloat;
    NSDictionary *mouseMoveInfo = [aNotification userInfo];
    NSEvent * event = [mouseMoveInfo objectForKey: @"1"];
    NSNumber *xNum =  [mouseMoveInfo objectForKey: @"2"];
    NSNumber *yNum =  [mouseMoveInfo objectForKey: @"3"];
    
    int height = [[NSScreen mainScreen] frame].size.height;
        
    dispatch_async(xtoqDispatchQueue, 
                   ^{ xtoq_mouse_motion (rootContext,
                                         [xNum intValue], 
                                         //Converting OSX coordinates to X11
                                         height - WINDOWBAR - [yNum intValue], 
                                         (int)[event windowNumber],
                                         0);;});

    
}

- (void) keyDownInView: (NSNotification *) aNotification
{   
    NSDictionary *keyInfo = [aNotification userInfo];
    // note this keyInfo is the key in <key, value> not the key pressed
    NSEvent * event = [keyInfo objectForKey: @"1"];
    unsigned short aChar = [event keyCode];
    NSString* charNSString = [event characters]; 
    const char* charcharstar = [charNSString UTF8String];

    NSLog(@"%s pressed", charcharstar);
    dispatch_async(xtoqDispatchQueue, 
                   ^{ xtoq_key_press(rootContext, 
                                     (int)[event windowNumber],
                                     aChar + 8) ;});
}

- (void) keyUpInView: (NSNotification *) aNotification
{   
    NSDictionary *keyInfo = [aNotification userInfo];
    // note this keyInfo is the key in <key, value> not the key pressed
    NSEvent * event = [keyInfo objectForKey: @"1"];
    unsigned short aChar = [event keyCode];

    dispatch_async(xtoqDispatchQueue, 
                   ^{ xtoq_key_release(rootContext, 
                                     (int)[event windowNumber],
                                     aChar + 8) ;});
}
 

// on this side all I have is a xtoq_context , on the library side I need
// to turn that into a real context 
- (void) mouseButtonDownInView: (NSNotification *) aNotification
{
    CGFloat heightFloat;
    NSDictionary *mouseDownInfo = [aNotification userInfo];
    // NSLog(@"Controller Got a XTOQmouseButtonDownEvent");
    NSEvent * event = [mouseDownInfo objectForKey: @"1"];

    NSNumber * heightAsNumber =  [NSNumber alloc];
    heightAsNumber = [mouseDownInfo objectForKey: @"2"];
    heightFloat = [heightAsNumber floatValue];
    //NSLog(@"Mouse Info: %@", [mouseDownInfo objectForKey: @"2"]);
    
    dispatch_async(xtoqDispatchQueue, 
                   ^{ xtoq_button_press (rootContext,
                                         0,
                                         0, 
                                         (int)[event windowNumber],
                                         0);;});
}

// on this side all I have is a xtoq_context , on the library side I need
// to turn that into a real context 
- (void) mouseButtonReleaseInView: (NSNotification *) aNotification
{
    CGFloat heightFloat;
    NSDictionary *mouseReleaseInfo = [aNotification userInfo];
    // NSLog(@"Controller Got a XTOQmouseButtonDownEvent");
    NSEvent * event = [mouseReleaseInfo objectForKey: @"1"];
    //NSRect bnd = NSMakeRect(0,0,512,386);
    NSNumber * heightAsNumber =  [NSNumber alloc];
    heightAsNumber = [mouseReleaseInfo objectForKey: @"2"];
    heightFloat = [heightAsNumber floatValue];
    //NSLog(@"Mouse Info: %@", [mouseDownInfo objectForKey: @"2"]);
    
    dispatch_async(xtoqDispatchQueue, 
                   ^{ xtoq_button_release (rootContext,
                                           0,
                                           0,
                                           (int)[event windowNumber],
                                           0);;});
}


- (void) setScreen:(char *)scrn {
    free(screen);
    screen = strdup(scrn);
    setenv("DISPLAY", screen, 1);
}

- (void) makeMenu {
    // Create menu  
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
    
    // Run Xterm
    NSMenuItem *xtermMenuItem;
    xTitle = @"Run Xterm";
    xtermMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle 
                                               action:@selector(runXterm:)
                                        keyEquivalent:@""];
    [startXMenu addItem:xtermMenuItem];
    [startXApps setSubmenu:startXMenu];
    
    // Run Xman
    NSMenuItem *xmanMenuItem;
    xTitle = @"Run Xman";
    xmanMenuItem = [[NSMenuItem alloc] initWithTitle:xTitle
                                              action:@selector(runXman:)
                                       keyEquivalent:@""];
    [startXMenu addItem:xmanMenuItem];
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
    
    newargv[0] = file_name;
    newargv[1] = "-display";
    newargv[2] = screen;
    newargv[3] = NULL;
    
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
-(void) runXman:(id) sender {
    setenv("MANPATH", "`manpath`", 1);
    [self launch_client:@"xman"];
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
	[newView setContext:windowContext];
    
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
- (void) updateImage:(xtoq_context_t *) windowContext
{
    float  y_transformed;
	NSRect newDamageRect;

    y_transformed =( windowContext->height - windowContext->damaged_y - windowContext->damaged_height)/1.0; 
	newDamageRect = NSMakeRect(windowContext->damaged_x,
							   y_transformed,
							   windowContext->damaged_width,
							   windowContext->damaged_height);
	XtoqView *localView = (XtoqView *)[(XtoqWindow *)windowContext->local_data contentView];
    [ localView setPartialImage:newDamageRect];
}

- (void) windowDidMove:(NSNotification*)notification {
    [self reshape];
}

- (void) windowDidResize:(NSNotification*)notification {
    [self reshape];
}

- (void) reshape {
    
  XtoqWindow *moveWindow = (XtoqWindow *)[NSApp mainWindow];
    
    if (moveWindow != nil) {        
        xtoq_context_t *moveContext = [moveWindow getContext];        
        NSRect moveFrame = [moveWindow frame];
        int x = (int)moveFrame.origin.x;
        int y = [self osxToXserver:(int)moveFrame.origin.y
					  windowHeight:moveContext->height] - WINDOWBAR;
        int width = (int)moveFrame.size.width;
        int height = (int)moveFrame.size.height - WINDOWBAR;
        NSLog(@"Call xtoq_configure_window(moveContext, x = %i, y = %i, height = %i, width = %i)", x, y, height, width); 
        xtoq_configure_window(moveContext, x, y - height, height, width);
		[[moveWindow contentView] setNeedsDisplay: YES];
    }    
}

@end

void eventHandler (xtoq_event_t *event)
{
    xtoq_context_t *context = event->context;
    if (event->event_type == XTOQ_DAMAGE) {
	  [referenceToSelf updateImage: context];
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
