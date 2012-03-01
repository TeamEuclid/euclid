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
#define RECTLOG(rect)    (NSLog(@""  #rect @" x:%f y:%f w:%f h:%f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height ));
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
        index = 0;
        indexTwo = 0;
        // Leaving these in for testing
        // file = @"Xtoq.app/Contents/Resources/Mac-Logo.jpg";
        // image2 = [[NSImage alloc] initWithContentsOfFile:(file)];   
        //[[self window] setAcceptsMouseMovedEvents:YES];
        //[[self window] makeFirstResponder:self];
        trackingArea = [[NSTrackingArea alloc] initWithRect:frame
                        
                                                    options: (NSTrackingMouseEnteredAndExited |
                                                              NSTrackingMouseMoved 
                                                              | NSTrackingActiveInKeyWindow
                                                              )
                        
                                                      owner:self userInfo:nil];
        
        [self addTrackingArea:trackingArea];
        
    }
    return self;
}

/**
 *  Overridden by subclasses to draw the receiver’s image within the passed-in rectangle.
 */
-(void)
drawRect:(NSRect)dirtyRect {

    //const NSRect ** rectList;
    //NSInteger * rectInt = 0;
    //[rectList count:rectInt];
    //NSLog(@"rectInt = %ld", (long)rectInt);
    
    /*const NSRect *rects;
    int count, i;
    id thing;
    
    NSEnumerator *thingEnumerator = [[self image] objectEnumerator];
    [self getRectsBeingDrawn:&rects count:&count];
    
    while (thing = [thingEnumerator nextObject]) {
        // First test against coalesced rect.
        if (NSIntersectsRect([thing bounds], dirtyRect)) {
            // Then test per dirty rect
            for (i = 0; i < count; i++) {
                if (NSIntersectsRect([thing bounds], rects[i])) {
                    [image draw];
                    break;
                }
            }
        }
    }
       }*/
    
    while (indexTwo < index) {
        int i = indexTwo++;
        [image[i] draw];//InRect:dirtyRect];
        [image[i] destroy];
    }
    index = indexTwo = 0;
    // Leaving in for testing
    //[image2 drawInRect:destRect fromRect:NSZeroRect
    //   operation:NSCompositeSourceOver fraction:1.0];
}

/**
 *  This is necessary for accepting input.
 */
- (BOOL)
acceptsFirstResponder {
    return YES;
}

- (BOOL)acceptsMouseMovedEvents {
    return YES;
}

- (void)mouseMoved:(NSEvent *)mouseMovedEvent
{ 
   // NSLog(@"mouse moved ------");

    NSRect bnd = [self bounds];
    CGFloat f = CGRectGetHeight(bnd);
    NSNumber *n = [[NSNumber alloc] initWithFloat:f];
    NSMutableDictionary *twoInfoDict = [[NSMutableDictionary alloc] initWithCapacity:2];
    [twoInfoDict setObject:mouseMovedEvent forKey:@"1"];
    [twoInfoDict setObject:n forKey:@"2"];
    
    //NSLog(@"bound %f location %f", CGRectGetHeight(bnd), [mouseMovedEvent locationInWindow].y );
    [notificationCenter postNotificationName:@"XTOQviewMouseMovedEvent" 
                                      object:self 
                                    userInfo:twoInfoDict];
}


- (void)mouseEntered:(NSEvent *)theEvent {
    
}

- (void)mouseExited:(NSEvent *)theEvent {
    
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
    CGFloat f = CGRectGetHeight(bnd);
    NSNumber *n = [[NSNumber alloc] initWithFloat:f];
    NSMutableDictionary *twoInfoDict = [[NSMutableDictionary alloc] initWithCapacity:2];
    [twoInfoDict setObject:mouseEvent forKey:@"1"];
    [twoInfoDict setObject:n forKey:@"2"];

    //NSLog(@"bound %f location %f", CGRectGetHeight(bnd), [mouseEvent locationInWindow].y );
    	
    [notificationCenter postNotificationName:@"XTOQmouseButtonDownEvent" 
                                      object:self 
                                    userInfo:twoInfoDict];
}

// This is getting called from the controller
- (void)setImage:(XtoqImageRep *)newImage {
    image[index++] = newImage;
}

- (void)setPartialImage:(XtoqImageRep *)newImage{

    image[index++] = newImage;
    //XtoqImageRep imageCopy = [[XtoqImageRep alloc] ]
    NSRect imageRec = NSMakeRect([newImage imageX], [newImage imageY], [newImage getWidth] , [newImage getHeight]);
    //RECTLOG(imageRec);
    //NSLog(@"index %i index 2 %i", index, indexTwo);
    //NSLog(@"frameRect = %@", NSStringFromRect(imageRec));    
    //[image drawInRect:imageRec];
    //[self ourDisp ];
    [self setNeedsDisplayInRect:imageRec];
    //[[self window] flushWindow];
}

- (BOOL)isOpaque{
    return YES;
}

- (void)ourDisp{
    CGContextRef destCtx = (CGContextRef)[[NSGraphicsContext currentContext]
                                          graphicsPort];
    while (indexTwo < index) {
        [image[indexTwo++] draw];//InRect:dirtyRect];
    }
}

@end
