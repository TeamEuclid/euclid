
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
#import "unistd.h"
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
    
    xtoq_context_t xcbContext;
   // xcb_image_t *imageT;
    
    xtoq_image_t *libImageT;
    XtoqImageRep *image;
    XtoqImageRep *imageNew;
    XtoqView *view;
    NSString *file;
    NSImage *image2;
    NSMutableDictionary *winList; // The window list data structure.
    NSInteger winCount;           // Used for setting keys of windows
    int originalHeight;
    int originalWidth;
    NSRect imageRec;
    NSString *keyFirst;
}

- (id) init;
- (void) applicationWillFinishLaunching:(NSNotification *) aNotification;
- (void) applicationDidFinishLaunching: (NSNotification *) aNotification;

- (void) keyDownInView: (NSNotification *) aNotification;

- (void) mouseButtonDownInView: (NSNotification *) aNotification;

/**
 * Makemenu and related selector functions for launching X applications.
 */
- (void) makeMenu;
- (void) runXeyes: (id) sender;
- (void) runXclock: (id) sender;
- (void) runXlogo: (id) sender;
- (void) runXterm: (id) sedner;
- (void) runXman: (id) sender;


/**
 * Create the Display Number Controller
 */

//- (IBAction)showDisplayChooser;

/**
 * Put a new image in the window / view
 */
- (void) updateImageNew: (xtoq_context_t *) windowContext;


- (void) createNewWindow: (xtoq_context_t *) windowContext;


/**
 * a method for getting a window in the list by searching all keys
 * returns nil if no key is found.
 */
- (XtoqWindow *) getWindowInList: (xtoq_context_t) xtoqContxt;

/**
 * A function that adds a window to the dictionary list.
 */
- (void) addWindowInList: (XtoqWindow *) xqWin 
             withContext: (xtoq_context_t) aContext;

/**
 * A function that removes a window from the list by key.
 */
- (void) removeWindowInLIst: (id) akey;

/**
 * Sets the screen to command line argument.
 */
- (void) setScreen: (const char *) scrn;



/**
 * Callback function that will receive events from the xtoq event loop
 * once it is started.
 */
void eventHandler (xtoq_event_t event);


//unsigned long long GetTimeSinceBoot;

@end
