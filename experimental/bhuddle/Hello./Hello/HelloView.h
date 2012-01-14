/** 
 *  HelloView.h
 *  Hello
 *
 *  Created by Aaron S. and Ben H. on January 12, 2012.
 *
 *  Create an window that displays an image from a file, 
 *  that toggles between another when the down arrow key is pressed. 
 */

#import <Cocoa/Cocoa.h>

@interface HelloView : NSView {
    NSString *file;
    NSString *file2;
    BOOL ourBool;
    NSImage *image;
    NSImage *image2;
}
@end

