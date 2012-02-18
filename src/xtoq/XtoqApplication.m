/*Copyright (C) 2012 Braden Wooley, Ben Huddle
 
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

#import "XtoqApplication.h"

@implementation XtoqApplication

int XtoqApplicationMain(int argc, char** argv){

    char *scrn;
    FILE *fp;
    const char *fifo_path = "/tmp/xtoq_fifo";
    
    // seems to error if file already exists
    mkfifo(fifo_path, S_IRUSR | S_IWUSR);
    
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        CFBundleRef bundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyBundleURL(bundle);
        CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
        char bundle_path[PATH_MAX];
        CFStringGetCString( str, bundle_path, PATH_MAX, kCFStringEncodingASCII );
        
        int len = strlen(bundle_path);
        bundle_path[len - 6] = 0;
        
        const char *startx_path = "/opt/X11/bin/startx";
        const char *Xorg_path = "/opt/X11/bin/Xorg";
        char *xinitrc_path;
        asprintf(&xinitrc_path, 
                 "%s/xtoq/Contents/Resources/X11/lib/X11/xinit/xinitrc", 
                 bundle_path);
        if (xinitrc_path == NULL) {
            //... perhaps create dir or exit program
            NSLog(@"xinitrc path is null");
        }
        
        const char *spawn[5];
        spawn[0] = startx_path;
        spawn[1] = xinitrc_path;
        spawn[2] = "--";
        spawn[3] = Xorg_path;
        spawn[4] = '\0';
        
        pid_t child;
        
        int error = posix_spawnp(&child, spawn[0], NULL, NULL, spawn, environ);
        if (error) {
            //... exit program if connection was not made.
            NSLog(@"error with posix_spawnp");
            exit(1);
        }
        
        int retval;
        waitpid(child, &retval, 0);
        if (retval != 0) {
            //... probably just exit here as well & set errno.
            NSLog(@"error with waitpid");
            exit(1);
        }
    });
    
    // Read $DISPLAY from XTOQ_DISPLAY_FIFO
    // Just giving up on this, too much time spent spinning wheels.
    // Display will default to :0

    char *xtoq_fifo = NULL;
    size_t bytes_read;
    size_t len = 0;
    
    fp = fopen(fifo_path, "r");
    if (fp == NULL) {
        NSLog(@"Problem with fp");
        //exit(1);
    }
    bytes_read = getline(&xtoq_fifo, &len, fp);
    xtoq_fifo[len - 1] = '\0';
    fclose(fp);
    scrn = ":0";
    //scrn = findScreen(xtoq_fifo);
    //setenv("DISPLAY", scrn, 1);   
    
    // Need this for some reason otherwise its too quick to segfault
    sleep(3);
    
    
    // initializes simple subclass
    [XtoqApplication sharedApplication];
    XtoqController *controller = [[XtoqController alloc] init];
    [controller setScreen:scrn];
    [NSApp setDelegate: controller];
    [NSApp run];

    return 1;
}

@end
