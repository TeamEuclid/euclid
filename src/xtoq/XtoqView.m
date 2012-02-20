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

#import "XtoqView.h"

@implementation XtoqView

/**
 *  This is the initializer.
 */
- (id)
initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    
    if (self) {
        notificationCenter = [NSNotificationCenter defaultCenter];
        [[self window] flushWindow];
        [self setNeedsDisplay:YES];
        
        // Leaving these in for testing
        // file = @"Xtoq.app/Contents/Resources/Mac-Logo.jpg";
        // image2 = [[NSImage alloc] initWithContentsOfFile:(file)];        
    }
    return self;
}

/**
 *  Overridden by subclasses to draw the receiver’s image within the passed-in rectangle.
 */
- (void)
drawRect:(NSRect)dirtyRect {

    /*
    const NSRect ** rectList;
    NSInteger * rectInt = 0;
    [rectList count:rectInt];
    NSLog(@"rectInt = %ld", (long)rectInt);*/
    
    [image draw];//InRect:dirtyRect];

    //[[self window] flushWindow];
    
    // Leaving in for testing
    //[image2 drawInRect:destRect fromRect:NSZeroRect
    //   operation:NSCompositeSourceOver fraction:1.0];
}


/**
 * Eventually implement- draw damaged rects
 */
/*- (void)getRectsBeingDrawn:(const NSRect **)rects count:(NSInteger *)count{
 
 //NSRect rect = NSMakeRect(10, 10, 100, 100);
 //[[NSColor purpleColor] setFill];
 //NSRectFill(rect); 
 
 //if (rectInt > 0){
    [image drawInRect:dirtyRect];
 //}
 [[self window] flushWindow];
 
 }*/


/**
 *  This is necessary for accepting input.
 */
- (BOOL)
acceptsFirstResponder {
    return YES;
}

/**
 *  Capture keyboard events
 */
- (void)
keyDown:(NSEvent *)theEvent {      
    NSDictionary * dictionary = [NSDictionary dictionaryWithObject:theEvent 
                                                            forKey:@"1"];

    [notificationCenter postNotificationName:@"XTOQviewKeyDownEvent" 
                                      object:self 
                                    userInfo:dictionary];
}

-(void)
mouseDown:(NSEvent *)mouseEvent {
    NSRect bnd = [self bounds];
    //NSPoint p = [mouseEvent locationInWindow];
    //downPoint = [self convertPoint:p fromView:nil];
    CGFloat f = CGRectGetHeight(bnd);
    NSNumber *n = [[NSNumber alloc] initWithFloat:f];
    NSMutableDictionary *twoInfoDict = [[NSMutableDictionary alloc] initWithCapacity:2];
    [twoInfoDict setObject:mouseEvent forKey:@"1"];
    [twoInfoDict setObject:n forKey:@"2"];

    NSLog(@"bound %f location %f", CGRectGetHeight(bnd), [mouseEvent locationInWindow].y );
    //[mouseEvent locationInWindow].y = 0;

    //NSDictionary * dictionary = [NSDictionary dictionaryWithObject:mouseEvent 
    //                                                        forKey:@"2"];
    	
    [notificationCenter postNotificationName:@"XTOQmouseButtonDownEvent" 
                                      object:self 
                                    userInfo:twoInfoDict];
}

// This is getting called from the controller
- (void)setImage:(XtoqImageRep *)newImage {
    image = newImage;
    //[[self window] flushWindow];
    //[self setNeedsDisplay:YES];
}

- (void)setPartialImage:(XtoqImageRep *)newImage x:(int)x y:(int)y dx:(int)dx dy:(int)dy{
    //sleep(1);
    image = newImage;
    NSLog(@" imageRec %i %i   %i %i", x,y,dx,dy );
    NSRect imageRec = NSMakeRect(x, y, dx, dy);
    
    //[image drawInRect:imageRec];
    //[[self window] flushWindow];
    [self setNeedsDisplayInRect:imageRec];
}

- (BOOL)isOpaque{
    return YES;
}

@end
