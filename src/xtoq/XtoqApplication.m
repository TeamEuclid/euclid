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
    FILE *fp;
    const char *fifo_path = "/tmp/xtoq_fifo";
    
    // seems to error if file already exists
    mkfifo(fifo_path, S_IRUSR | S_IWUSR);    
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        CFBundleRef bundle = CFBundleGetMainBundle();
        assert(bundle);

        CFURLRef resourcesURL = CFBundleCopyBundleURL(bundle);
        assert(resourcesURL);

        CFStringRef bundle_path_cfstr = CFURLCopyFileSystemPath(resourcesURL, kCFURLPOSIXPathStyle);
        assert(bundle_path_cfstr);

        char bundle_path[PATH_MAX];
        CFStringGetCString(bundle_path_cfstr, bundle_path, PATH_MAX, kCFStringEncodingUTF8);
        
        const char *startx_path = "/opt/X11/bin/startx";
        const char *Xorg_path = "/opt/X11/bin/Xorg";
        char *xinitrc_path;
        asprintf(&xinitrc_path, 
                 "%s/Contents/Resources/X11/lib/X11/xinit/xinitrc", 
                 bundle_path);
        assert(xinitrc_path);
        
        const char *spawn[5];
        spawn[0] = startx_path;
        spawn[1] = xinitrc_path;
        spawn[2] = "--";
        spawn[3] = Xorg_path;
        spawn[4] = NULL;
        
        pid_t child;

        NSLog(@"Executing: %s %s %s %s", spawn[0], spawn[1], spawn[2], spawn[3]);
        
        int error = posix_spawnp(&child, spawn[0], NULL, NULL, (char * const *)spawn, environ);
        if (error) {
            //... exit program if connection was not made.
            NSLog(@"error with posix_spawnp");
            exit(1);
        }
        
        int retval;
        waitpid(child, &retval, 0);
        if (retval != 0) {
            NSLog(@"The X11 server shutdown.");
            exit(EXIT_SUCCESS);
        }
    });
    
    // Read $DISPLAY from XTOQ_DISPLAY_FIFO
    // Just giving up on this, too much time spent spinning wheels.
    // Display will default to :0
    char *scrn = NULL;
    ssize_t bytes_read;
    size_t len = 0;
    
    fp = fopen(fifo_path, "r");
    assert(fp);
#if 0
    bytes_read = getline(&scrn, &len, fp);
#else
    scrn=":0";
#endif
    fclose(fp);
    scrn[len - 1] = '\0';

    NSLog(@"xinitrc told us DISPLAY=%s\n", scrn);
    
    // initializes simple subclass
    [XtoqApplication sharedApplication];
    XtoqController *controller = [[XtoqController alloc] init];
    [controller setScreen:scrn];
    [NSApp setDelegate: controller];
    [NSApp run];

    return 1;
}

@end
