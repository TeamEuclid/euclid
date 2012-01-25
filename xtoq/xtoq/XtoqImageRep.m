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
    imageTypes = [[NSArray alloc] initWithObjects:@".xbm",@".ppm",@".bmp",nil]; 
    return imageTypes;
}

- (BOOL)canInitWithData:(NSData *)data {
    return YES;
}

//- (id)initWithData:(NSData *)data{
- (id)init{
    //Replace :1 with the popup window var
    screen = ":1";
    xcbContext = Init(screen);
    imageT = GetImage(xcbContext);
       NSLog(@"data Files stuff in itit with data:  %i", imageT->size);
    windowSize =  NSMakeSize(imageT->width, imageT->size);
    // might want to use initWithBytesNoCopy:length
    //theData = [[NSData alloc] initWithBytes:imageT->data length:imageT->size];
 
    
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
    //?ImageIO: <ERROR>  CGImageSourceCreateWithData data parameter is nil? - this is fixed
    cgImage = CGImageCreate (
 /*                          size_t width,
                           size_t height,
                           size_t bitsPerComponent,
                            size_t bitsPerPixel,
                           size_t bytesPerRow,
                           CGColorSpaceRef colorspace,
                           CGBitmapInfo bitmapInfo,
                           CGDataProviderRef provider,
                           const CGFloat decode[],
                           bool shouldInterpolate,
                           CGColorRenderingIntent intent
                           );*/
                            imageT->width,// size_t width,
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
            NSLog(@"bbbbbbb");
    return self;
}

- (BOOL)draw{
        NSLog(@"11111111");
    //CGContextRef contextMac = [self MyCreateBitmapContext :780 :560];
    //CGContextRef contextMac = [[NSGraphicsContext currentContext] graphicsPort];
    //if (contextMac == NULL)
    //            NSLog(@"NULLLLll");
	if ( !cgImage) {
        NSLog(@"bang image");
		return NO;
	}
	NSSize size = [self size];
	//CGContextDrawImage(contextMac, CGRectMake(0, 0, size.width, size.height), cgImage);
        NSLog(@"999999");
	return YES;

}



- (BOOL)drawInRect:(NSRect)rect {
    
    NSLog(@"asdf");

    
    [[NSGraphicsContext currentContext]
     setImageInterpolation:NSImageInterpolationHigh];

    nsImage = [[NSImage alloc] initWithCGImage:cgImage size:windowSize];
    
  //  unsigned char ** wtf;
   // wtf[0] = imageT->data;
    //myBitMapRep = [[NSBitmapImageRep alloc] initWithData:theData];
    
    /*myBitMap = [[NSBitmapImageRep alloc]             
                initWithBitmapDataPlanes: (unsigned char **) wtf
                pixelsWide:imageT->width 
                pixelsHigh:imageT->height 
                bitsPerSample:8 
                samplesPerPixel:3 
                hasAlpha:NO 
                isPlanar:NO 
                colorSpaceName:NSDeviceRGBColorSpace 
                bytesPerRow:(imageT->width)*3 
                bitsPerPixel:8*3]; */

    [nsImage drawInRect:rect fromRect:NSZeroRect 
              operation:NSCompositeSourceOver fraction:1.0];
    return YES;
}
@end
