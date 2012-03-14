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

        trackingArea = [[NSTrackingArea alloc] initWithRect:frame
                        options: (NSTrackingMouseEnteredAndExited |
                        NSTrackingMouseMoved 
                        | NSTrackingActiveInKeyWindow)
                                                      owner:self userInfo:nil];
        [self addTrackingArea:trackingArea];
        
    }
    return self;
}

-(void) setContext: (xtoq_context_t *)context {
    viewContext = context;
}

// Overridden by subclasses to draw the receiver’s image within the
// passed-in rectangle.
-(void)
drawRect:(NSRect)dirtyRect {
    xtoq_image_t *imageT;
	float y_transformed;
	XtoqImageRep *imageNew;

    xtoq_get_event_thread_lock();
    imageT = test_xtoq_get_image(viewContext);
	if (imageT->image) {
        y_transformed =( viewContext->height
						 - viewContext->damaged_y
						 - viewContext->damaged_height)/1.0; 
		imageNew = [[XtoqImageRep alloc]
					 initWithData:imageT
					            x:((viewContext->damaged_x))
					            y:y_transformed];
		[imageNew draw];
		[imageNew destroy];
		
		// Remove the damage
		xtoq_remove_context_damage(viewContext);
	}
	xtoq_release_event_thread_lock();
}



//This is necessary for accepting input.
- (BOOL)
acceptsFirstResponder {
    return YES;
}

- (BOOL)acceptsMouseMovedEvents {
    return YES;
}

/*- (void)mouseEntered:(NSEvent *)theEvent {

}

- (void)mouseExited:(NSEvent *)theEvent {

}
 
- (void)rightMouseDown:(NSEvent *)theEvent 

}*/

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
    CGFloat f = [self bounds].size.height;
    NSNumber *n = [[NSNumber alloc] initWithFloat:f];
    //NSLog(@"mouseevent %i", [mouseEvent mouseLocation]->x);
   // NSLog(@"mouse event bound %f location %f", CGRectGetHeight(bnd), [mouseEvent locationInWindow].y );
    NSMutableDictionary *twoInfoDict = [[NSMutableDictionary alloc] initWithCapacity:2];
    [twoInfoDict setObject:mouseEvent forKey:@"1"];
    [twoInfoDict setObject:n forKey:@"2"];

    //NSLog(@"bound %f location %f", CGRectGetHeight(bnd), [mouseEvent locationInWindow].y );
    [notificationCenter postNotificationName:@"XTOQmouseButtonDownEvent" 
                                      object:self 
                                    userInfo:twoInfoDict];
}

- (void)mouseUp:(NSEvent *)theEvent {
	CGFloat f = [self bounds].size.height;
    NSNumber *n = [[NSNumber alloc] initWithFloat:f];
    NSMutableDictionary *twoInfoDict = [[NSMutableDictionary alloc] initWithCapacity:2];
    [twoInfoDict setObject:theEvent forKey:@"1"];
    [twoInfoDict setObject:n forKey:@"2"];
    [notificationCenter postNotificationName:@"XTOQmouseButtonReleaseEvent" 
                                      object:self 
                                    userInfo:twoInfoDict];
}

- (void)setPartialImage:(NSRect)newDamageRect {    
    [self setNeedsDisplayInRect:newDamageRect];
}

- (BOOL)isOpaque{
    return YES;
}

@end
