
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
    [ourView setPartialImage:imageNew];
    
    
    // add root window to list, increment count of windows
    keyFirst = [NSString stringWithFormat:@"%d", winCount];
    [xtoqWindow setContext:xcbContext withId:keyFirst];
    [xtoqWindow setRootDataPointer:xcbContext];
    [winList setObject:xtoqWindow forKey:keyFirst];
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
                   ^{ dummy_xtoq_button_down (xcbContext,
                                        [event locationInWindow].x, 
                                        heightFloat - [event locationInWindow].y, 
                                        (int)[event windowNumber],
                                        0);;});
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
                  //NSMakeRect(0, 0, 1024,768)
                  NSMakeRect(0, 0, 1024,768)
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
    xcbImage = xtoq_get_image(xcbContext);
    imageRep = [[XtoqImageRep alloc] initWithData:xcbImage x:0 y:0];
    
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

- (void) updateImageNew : (xtoq_context_t *) windowContext{
    float x_transformed, y_transformed, dWidth,dHeight;
    int numberOfRects = 1;
	int i;
        
    //if (libImageT.image)
    //    xcb_image_destroy(libImageT.image);
    libImageT = test_xtoq_get_image(*windowContext);
    NSLog(@"update image new values in - %i, %i, %i, %i", windowContext->damaged_x, windowContext->damaged_y, windowContext->damaged_width, windowContext->damaged_height);

    y_transformed =( windowContext->height - windowContext->damaged_y - windowContext->damaged_height)/1.0; 
    imageNew = [[XtoqImageRep alloc] initWithData:libImageT.image
                                                    x:((windowContext->damaged_x))
                                                    y:y_transformed];
    NSLog(@"NSS %lu", GetTimeSinceBoot());
    [ourView setPartialImage:imageNew];

}



unsigned long long GetTimeSinceBoot()
{
    UnsignedWide uw = AbsoluteToNanoseconds(UpTime());
    return ((((unsigned long long)uw.hi)<<32)|(uw.lo));
}


void eventHandler (xtoq_event_t event)
{
    if (event.event_type == XTOQ_DAMAGE) {
        // This message generates a lot of console spam - only uncomment when testing
        //NSLog(@"Got damage event");
        
        [referenceToSelf updateImageNew: event.context];
    } else if (event.event_type == XTOQ_CREATE) {
        NSLog(@"Window was created");
        [referenceToSelf createNewWindow: event.context];
    } else if (event.event_type == XTOQ_DESTROY) {
        [referenceToSelf updateImageNew: event.context];
    } else { 
        NSLog(@"Hey I'm Not damage!"); 
    }
    
}

@end


