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
    struct CGImage *cgImage;
    xcb_image_t *imageT;
    xtoq_image_t * imageParent;
    NSSize size;
    CGFloat width;
    CGFloat height;
    NSArray *imageTypes;
    NSSize windowSize; 
    float imageX;
    float imageY;
    
}

- (float)imageX;
- (float)imageY;


/**
 * Return an NSArray of supported file types, currently nil
 * @return An array of supported image file types.
 */
- (NSArray *)imageUnfilteredTypes;


/**
 * Return Yes if the xcb_image_t it is passed is not null, else No.
 * @param imageData The image from the Xserver that you want drawn
 * @return No if image from the xserver is null else Yes
 */
- (BOOL)canInitWithData:(xcb_image_t *)imageData;

/**
 * Return the image it has constructed from the imageData
 * @param imageData The image from the Xserver that you want drawn
 * @return id It returns its own id
 */
- (id)initWithData:(xtoq_image_t *)imageData x:(int)x y:(int)y;

/**
 * Return whether the window was drawn
 * 'draw' draws the image at the coordinate system's origin 
 * @return bool Yes if the image was drawn, No if it was not.
 */
- (BOOL)draw;

/**
 * Return an NSSize with the dimensions of the image
 * @return NSSize with the dimensions of the image
 */
- (NSSize)size;

- (CGFloat)getWidth;

- (CGFloat)getHeight;
- (BOOL)drawInRect:(NSRect)rect;

- (void)destroy;
@end
