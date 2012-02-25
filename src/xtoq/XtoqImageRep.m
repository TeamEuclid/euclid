/* Copyright (c) 2012 Aaron Skomra and Braden Wooley
 *
 * XtoqImageRep.m
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

#import "XtoqImageRep.h"

@implementation XtoqImageRep

- (NSArray *)imageUnfilteredTypes{
    imageTypes = [[NSArray alloc] initWithObjects:nil]; 
    return imageTypes;
}

- (BOOL)canInitWithData:(xcb_image_t *)imageData {
    if (imageData == NULL) {
        return NO;
    }    
    return YES;
}


- (id)initWithData:(xtoq_image_t *)imageData x:(int)x y:(int)y{
    // We might need implement GetSize
    imageParent = imageData;
    imageT = imageData->image;
    windowSize =  NSMakeSize(imageT->width, 	imageT->size);

    self = [super init];
	if (!self) {
		return nil;
    }
    CGDataProviderRef cgdat =  CGDataProviderCreateWithData (
                                                    NULL,
                                                    imageT->data,
                                                    imageT->size,
                                                    NULL
                                                    );
    
    CGColorSpaceRef csp = CGColorSpaceCreateDeviceRGB();

    cgImage = CGImageCreate (imageT->width,// size_t width,
                             imageT->height, //size_t height,
                             8, //size_t bitsPerComponent,
                             32,//size_t bitsPerPixel,
                             imageT->stride,//size_t bytesPerRow,
                             csp, //CGColorSpaceRef colorspace,
                             kCGBitmapByteOrderDefault,//CGBitmapInfo bitmapInfo,
                             cgdat,//CGDataProviderRef provider,
                             NULL, //const CGFloat decode[],
                             YES, //bool shouldInterpolate,
                             kCGRenderingIntentDefault //CGColorRenderingIntent intent
                             );
    width = CGImageGetWidth(cgImage);
    height = CGImageGetHeight(cgImage);
    size = NSMakeSize (width, height);
    imageX =x;
    imageY =y;
    return self;
}

// testing function
- (id)topCrop{
    CGSize sizeH = [self size];
    CGRect rect = CGRectMake(0, 30,                      
                sizeH.width - 30, sizeH.height-30 );
    
    CGImageRef imageRef = CGImageCreateWithImageInRect(cgImage, rect);
    cgImage = imageRef;
    return self;
}


- (BOOL)draw{   
    CGContextRef contextMac = [[NSGraphicsContext currentContext] graphicsPort];
    if (contextMac == NULL)
        NSLog(@"NULL context in draw");
	if ( !cgImage) {
        NSLog(@"No image");
		return NO;
	}
    //    CGContextDrawImage(contextMac, CGRectMake(imageX, imageY, width, height), cgImage);
    CGContextDrawImage(contextMac, CGRectMake(imageX, imageY, width, height), cgImage);
    
	return YES;
}

- (BOOL)drawInRect:(NSRect)rect{   
    CGContextRef contextMac = [[NSGraphicsContext currentContext] graphicsPort];
    if (contextMac == NULL)
        NSLog(@"NULL context in drawInRect");
	if ( !cgImage) {
        NSLog(@"No image");
		return NO;
	}
    //changed origin to imageX imageY to get damage to work
    CGContextDrawImage(contextMac, CGRectMake(imageX, imageY, width, height), cgImage);
    
	return YES;
}

- (NSSize)size{
    return size;
}

- (CGFloat)getWidth{
    return width;
}

- (CGFloat)getHeight{
    return height;
}

- (float)imageX{
    return imageX;
}
- (float)imageY{
    return imageY;
}
- (void)destroy{
    if (imageParent) {
        xtoq_image_destroy(imageParent);
    }
}
@end
