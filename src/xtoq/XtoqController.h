
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
#include <CoreServices/CoreServices.h> // timestamp
#import <AppKit/AppKit.h>
#import "XtoqWindow.h"
#import "XtoqImageRep.h"
#import "XtoqView.h"
#import "xtoq.h"
#import "spawn.h"
#import "crt_externs.h"
#import "sys/times.h"
#import "sys/stat.h"
#import "unistd.h"
#import <dispatch/dispatch.h>

#ifdef __APPLE__
#define environ (*_NSGetEnviron())
#endif

id referenceToSelf;

@interface XtoqController : NSObject {
    XtoqWindow *xtoqWindow;
    XtoqView * ourView;
    
    dispatch_queue_t xtoqDispatchQueue;
    
    //The X :1 paramater, updated in the XtoqApplication
    char *screen;
    
    xtoq_image_t *libImageT;
    xtoq_context_t *rootContext;
    xcb_image_t *imageT;
    XtoqImageRep *image;
    XtoqImageRep *imageNew;
    XtoqView *view;

    int originalHeight;
    int originalWidth;
    NSRect imageRec;
    NSString *keyFirst;
}

- (id) init;
- (void) applicationWillFinishLaunching:(NSNotification *) aNotification;
- (void) applicationDidFinishLaunching: (NSNotification *) aNotification;

/**
 * Receive notification of a key down event from the view.
 * @param an NSNotification containing an NSEvent 
 */
- (void) keyDownInView: (NSNotification *) aNotification;

/**
 * Receive notification of a key up event from the view.
 * @param an NSNotification containing an NSEvent 
 */
- (void) keyUpInView: (NSNotification *) aNotification;

/**
 * Receive notification of a mouse button press from the view.
 * @param an NSNotification containing an NSEvent 
 */
- (void) mouseButtonDownInView: (NSNotification *) aNotification;

/**
 * Receive notification of a mouse button release from the view.
 * @param an NSNotification containing an NSEvent 
 */
- (void) mouseButtonReleaseInView: (NSNotification *) aNotification;

/**
 * Makemenu and related selector functions for launching X applications.
 */
- (void) makeMenu;

/**
 * Launches the application based on filename.
 *
 * Launches the selected application based on the filename passed in as an
 * argument.  Uses posix_spawn() to launch the application after the arguments
 * have been filled in.
 *
 * @param filename The name of the application to be run within XtoQ.app.
 */
- (void) launch_client: (NSString *) filename;

/**
 * Runs xeyes.
 *
 * Sends "xeyes" argument to the launch client function to open up the
 * application xeyes within XtoQ.app.
 *
 * @param sender Only needed for functionality with Makemenu's menu system.
 */
- (void) runXeyes: (id) sender;

/**
 * Runs xclock.
 *
 * Sends "xclock" argument to the launch client function to open up the
 * application xclock within XtoQ.app.
 *
 * @param sender Only needed for functionality with Makemenu's menu system.
 */
- (void) runXclock: (id) sender;

/**
 * Runs xlogo.
 *
 * Sends "xlogo" argument to the launch client function to open up the
 * application xlogo within XtoQ.app.
 *
 * @param sender Only needed for functionality with Makemenu's menu system.
 */
- (void) runXlogo: (id) sender;

- (void) mouseMovedInApp: (NSNotification *) aNotification;

/**
 * Runs xterm.
 *
 * Sends "xterm" argument to the launch client function to open up the
 * application xterm within XtoQ.app.
 *
 * @param sender Only needed for functionality with Makemenu's menu system.
 */
- (void) runXterm: (id) sender;

/**
 * Runs xman
 *
 * Sends "xman" argument to the launch client function to open up the
 * application xman within XtoQ.app.
 *
 * @param sender Olny needed for functionality with Makemenu's menu system.
 */
-(void) runXman: (id) sender;

/**
 * Put a new image in the window / view
 * Send an image to the view after being notified of a damage event from 
 * the event handler.
 * @param an xtoq_context_t sent from eventHandler
 */
- (void) updateImage: (xtoq_context_t *) windowContext;

- (void) createNewWindow: (xtoq_context_t *) windowContext;
- (void) destroyWindow:   (xtoq_context_t *) windowContext;

/**
 * Sets the variable screen to correct display.
 *
 * Sets class variable screen to correct display and also sets the environment
 * variable "DISPLAY" to correct value.
 *
 * @param scrn This value is determined in XtoqApplication before connection
 *   to Xorg is established.
 */
- (void) setScreen: (char *) scrn;

- (void)windowWillMove:(NSNotification*)notification;
- (void)windowDidMove:(NSNotification*)notification;
- (void)windowDidResize:(NSNotification*)notification;
- (void)applicationWillTerminate:(NSNotification *)aNotification;
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
