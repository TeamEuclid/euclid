
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
#import "DisplayNumberController.h"
#import "XtoqWindow.h"
#import "XtoqImageRep.h"
#import "XtoqView.h"
#import "xtoq.h"

@class DisplayNumberController;

@interface XtoqController : NSObject {
    DisplayNumberController *displayNumberController;
    XtoqWindow *xtoqWindow;
    XtoqView * ourView;
    
    //The X :1 paramater
    char *screen;
    
    xtoq_context_t xcbContext;
    xcb_image_t *imageT;
    XtoqImageRep *image;
    XtoqView *view;
    NSString *file;
    NSImage *image2;
    NSMutableDictionary *winList; // The window list data structure.
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification;
- (void) applicationDidFinishLaunching: (NSNotification *) aNotification;

- (void) keyDownInView: (NSNotification *) aNotification;

/**
 * Create the Display Number Controller
 */

- (IBAction)showDisplayChooser;

/**
 * Wait for the event.
 */
- (void) wait_for_xtoq_event;

/**
 * Put a new image in the window / view
 */
- (void) updateImage;

/**
 * a function for setting a window in the window list
 * might need to be modified to actually set context if possible.
 */
- (void) setWindowInList: (XtoqWindow *) windowId forKey: (id) akey;

/**
 * a method for getting a window in the list by searching all keys
 * returns nil if no key is found.
 */
- (XtoqWindow *) getWindowInList: (id) aKey 
                      withContxt: (xtoq_context_t) xtoqContxt;

@end
