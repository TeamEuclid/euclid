/** 
 *  HelloView.m
 *  Hello
 *
 *  Created by Aaron S. and Ben H. on January 12, 2012.
 *
 *  Create an window that displays an image from a file, 
 *  that toggles between another when the down arrow key is pressed. 
 *
 *  Note: The image used was hardcoded in the directory we used.
 *  It must be changed to the current users directory.
 */

#import "HelloView.h"

@implementation HelloView

/**
 *  This is the initializer.
 */
- (id)
initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    
    if (self) {
        ourBool = YES;
        // This must be changed to users personal directory.
        file = @"/Users/aas/learn/Hello./Mac-Logo.jpg";
        file2 = @"/Users/aas/learn/Hello./Mac-Logo2.jpg"; 
        image = [[NSImage alloc] initWithContentsOfFile:file];
        image2 = [[NSImage alloc] initWithContentsOfFile:file2];
    }
    
    return self;
}

/**
 *  This function draws the initial image to the window.
 */
- (void)
drawRect:(NSRect)dirtyRect {
    [[NSGraphicsContext currentContext]
     setImageInterpolation:NSImageInterpolationHigh];
    
    NSSize imageSize = { 350, 300 };
    NSRect destRect;
    destRect.size = imageSize;
    [image drawInRect:destRect fromRect:NSZeroRect 
            operation:NSCompositeSourceOver fraction:1.0];
}

/**
 *  This is necessary for accepting input.
 */
- (BOOL)
acceptsFirstResponder {
    return YES;
}

/**
 *  This is the function that captures the event which is 
 *  the down arrow key, not the numpad down arrow key.
 *  It changes the image on the screen when the down arrow is pressed.
 */
- (void)
keyDown:(NSEvent *)theEvent {
    NSString *characters = [theEvent characters];
    int key = [characters characterAtIndex:0];
    NSSize imageSize = { 350, 300 };
    NSRect destRect;
    destRect.size = imageSize;
    
    if (key == NSDownArrowFunctionKey) {
        if (ourBool == YES) {
            ourBool = NO;
            [image2 drawInRect:destRect fromRect:NSZeroRect 
                     operation:NSCompositeSourceOver fraction:1.0];
        } else {
            ourBool = YES;
            [image drawInRect:destRect fromRect:NSZeroRect 
                    operation:NSCompositeSourceOver fraction:1.0];
        }
        [[self window] flushWindow];
    } else {
        [super keyDown:theEvent];
    }
}

@end

