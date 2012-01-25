/* Copyright (c) 2012 Aaron Skomra and Braden Wooley
 *
 * XtoqImageRep.h
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <xcb/xcb_image.h>
#import "xtoq.h"


@interface XtoqImageRep : NSImageRep {
    NSArray *imageTypes;
    //NSImageRep *imageForDisplay;
    struct CGImage *cgImage;
    NSBitmapImageRep *myBitMapRep;
    //CGImageRef image;
    // xcb connection
    char *screen;
    context_t xcbContext;
    xcb_image_t *imageT;
    NSData *theData;
    NSSize windowSize;
    
    
    
    //aaron test
    NSString * file;
    NSImage * junkImage;
    NSImage *nsImage;
    
    
}
/**
 * Returns an array of UTI strings identifying the image types 
 * supported directly by the receiver. 
 * @return array An array of NSStrings containing UTI identifying 
 *          supported image types.
 */
- (NSArray *)imageUnfilteredTypes;

/**
 * Returns a Boolean value indicating whether the receiver can 
 * initialize itself from the specified data.
 * @param data The NSData object containing the image data.
 * @return BOOL YES if the receiver understands the format 
 *          of the specified data and can use it to initialize 
 *          itself; otherwise, NO.
 */
- (BOOL)canInitWithData:(NSData *)data;

/**
 * Initializes and returns an NSImage instance with the contents 
 * of the specified NSData object.
 * @param data The NSData object containing the image data. 
 * @return id An initialized NSImage instance, or nil if the 
 *          method cannot create an image representation from the
 *          contents of the specified data object.
#import <Cocoa/Cocoa.h> */
//- (id)initWithData:(NSData *)data;
- (id)init;

/**
 * Draws the image in the current coordinate system.
 * @return BOOL YES if the image was successfully drawn; otherwise,
 *          NO if there was a problem.
 */
- (BOOL)draw;

- (BOOL)drawInRect:(NSRect)rect;



@end
