/*Copyright (C) 2012 Aaron Skomra and Ben Huddle

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
 *  XtoqView.h
 *  Xtoq
 *
 *  Create an window that displays an image from a file, 
 *  that toggles between another when the down arrow key is pressed. 
 */

#import <Cocoa/Cocoa.h>
#import "xtoq_internal.h"
#import "XtoqImageRep.h"
#import "xtoq.h"

@interface XtoqView : NSView {
    XtoqImageRep *image[10000];  
    
    
    NSImage *image2;
    NSString *file;
    
    xcb_image_t *imageT;

    //mouse event 
    NSPoint downPoint;
    NSPoint currentPoint;
    
    NSNotificationCenter * notificationCenter;
    
    //junk
    int index;
    int indexTwo;
}

- (void)setImage:(XtoqImageRep *)newImage;
- (void)setPartialImage:(XtoqImageRep *)newImage;
- (id)initWithFrame:(NSRect)frame;
//- (void)getRectsBeingDrawn:(const NSRect **)rects count:(NSInteger *)count;
- (void)ourDisp;
@end

