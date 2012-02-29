
/*Copyright (C) 2012 Aaron Skomra, Braden Wooley, Ben Huddle
 
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
 *  AppController.h
 *  xtoq
 *
  *  TODO: rename this class to XtoqController
 *  This was controller for the Popup to retreive the display number
 *  from the user.
 *
 *  This is the window controller // clean up some of the names
 */

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#import "XtoqWindow.h"
#import "XtoqImageRep.h"
#import "XtoqView.h"
#import "xtoq.h"
#import <dispatch/dispatch.h>

@class DisplayNumberController;

id referenceToSelf;

@interface XtoqController : NSObject {
    DisplayNumberController *displayNumberController;
    XtoqWindow *xtoqWindow;
    XtoqView * ourView;
    
    dispatch_queue_t xtoqDispatchQueue;
    
    //The X :1 paramater, updated in the XtoqApplication
    char *screen;
    
    xtoq_context_t *rootContext;
    xcb_image_t *imageT;
    XtoqImageRep *image;
    XtoqView *view;
    NSString *file;
    NSImage *image2;
   // NSMutableDictionary *winList; // The window list data structure.
   // NSInteger winCount;           // Used for setting keys of windows
    int originalHeight;
    int originalWidth;
}

- (id) init;
- (void) applicationWillFinishLaunching:(NSNotification *) aNotification;
- (void) applicationDidFinishLaunching: (NSNotification *) aNotification;

- (void) keyDownInView: (NSNotification *) aNotification;

- (void) mouseButtonDownInView: (NSNotification *) aNotification;
- (void) makeMenu;

/**
 * Put a new image in the window / view
 */
- (void) updateImage;
- (void) createNewWindow: (xtoq_context_t *) windowContext;
- (void) destroyWindow:   (xtoq_context_t *) windowContext;

/**
 * Sets the screen to command line argument.
 */
- (void) setScreen: (char *) scrn;

- (void)windowWillMove:(NSNotification*)notification;
- (void)windowDidMove:(NSNotification*)notification;
- (void)windowDidResize:(NSNotification*)notification;
- (void)reshape;

- (int) xserverToOSX:(int)yValue windowHeight:(int)windowH;
- (int) osxToXserver:(int)yValue windowHeight:(int)windowH;

@end

/**
 * Callback function that will receive events from the xtoq event loop
 * once it is started.
 * @param event The event received.
 */
void eventHandler (xtoq_event_t *event);
