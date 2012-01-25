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
        ourBool = YES;
        // This must be changed to users personal directory.
        //file = @"Xtoq.app/Contents/Resources/Mac-Logo.jpg";
        //image = [[NSImage alloc] initWithContentsOfFile:file];
  //      context = Init(screen);
  //      imageT = GetImage(context);
        // might want to use initWithBytesNoCopy:length
 //       data = [[NSData alloc] initWithBytes:imageT->data length:imageT->size];
 //       image = [[XtoqImageRep alloc] initWithData:data];
        //    NSLog(@"VIEW data Files stuff:  %i", imageT->size);
        image = [[XtoqImageRep alloc] init];
        [image draw];
        file2 = @"Xtoq.app/Contents/Resources/Mac-Logo2.jpg";
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
    NSSize imageSize = { 1024,768 };
    NSRect destRect;
    destRect.size = imageSize;
    [image drawInRect:destRect];
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
    [[NSGraphicsContext currentContext]
     setImageInterpolation:NSImageInterpolationHigh];
    
    NSString *characters = [theEvent characters];
    int key = [characters characterAtIndex:0];
    NSSize imageSize = { 1024, 768};
    NSRect destRect;
    destRect.size = imageSize;
    
    if (key == NSDownArrowFunctionKey) {
        if (ourBool == YES) {
            ourBool = NO;
            [image2 drawInRect:destRect fromRect:NSZeroRect 
                     operation:NSCompositeSourceOver fraction:1.0];
        } else {
            ourBool = YES;
                [image drawInRect:destRect];
            //[image drawInRect:destRect]; //fromRect:NSZeroRect 
                    //operation:NSCompositeSourceOver fraction:1.0];
        }
        [[self window] flushWindow];
    } else {
        [super keyDown:theEvent];
    }
}

@end

