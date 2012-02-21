/*Copyright (C) 2012 Ben Huddle
 
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

// Placehoder

#import <Cocoa/Cocoa.h>
#import "xtoq.h"
#import "XtoqView.h"

@interface XtoqWindow : NSWindow {
    xtoq_context_t *winContext;  // The context of the window.
    id winId;                   // String id of window in list (i.e. 0,1,2,3)
    void *xtoqLocalData;        // Pointer to xtoq_context's data structure.
    NSNotificationCenter * notificationCenter;
}

/**
 * Used for initialization of a window.
 */
-(id) initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle 
                  backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag;

/*
 * Orders a specific window to the front and makes it key window.
 */
-(void) makeKeyAndOrderFront:(id)sender;

/**
 * Used for setting member variables context and id.
 */
-(void) setContext: (xtoq_context_t *) aContext withId: (id) theId;

/**
 * Function for getting context of window from list.
 */
-(xtoq_context_t *) getContext:(XtoqWindow *) win;

/**
 * Sets the root window's pointer xtoqLocalData to the context's pointer.
 */
-(void) setRootDataPointer: (xtoq_context_t *) xqContext;

- (BOOL)windowShouldClose:(id)sender;


@end
