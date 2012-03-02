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

    XtoqController *controller;
    char *scrn;
    FILE *fp;
    const char *fifo_path = "/tmp/xtoq_fifo";
    ssize_t bytes_read;
    size_t len = 0;
    char bundle_path[PATH_MAX];
    const char *startx_path = "/opt/X11/bin/startx";
    const char *Xorg_path = "/opt/X11/bin/Xorg";
    char *xinitrc_path;
    const char *spawn[5];
    pid_t child;
    

    mkfifo(fifo_path, S_IRUSR | S_IWUSR);    
    

    CFBundleRef bundle = CFBundleGetMainBundle();
    assert(bundle);

    CFURLRef resourcesURL = CFBundleCopyBundleURL(bundle);
    assert(resourcesURL);

    CFStringRef bundle_path_cfstr = CFURLCopyFileSystemPath(resourcesURL, kCFURLPOSIXPathStyle);
    assert(bundle_path_cfstr);

        
    CFStringGetCString(bundle_path_cfstr, bundle_path, PATH_MAX, kCFStringEncodingUTF8);
        
        
    asprintf(&xinitrc_path, 
            "%s/Contents/Resources/X11/lib/X11/xinit/xinitrc", 
            bundle_path);
    assert(xinitrc_path);
        
        
    spawn[0] = startx_path;
    spawn[1] = xinitrc_path;
    spawn[2] = "--";
    spawn[3] = Xorg_path;
    spawn[4] = NULL;
        
    int error = posix_spawnp(&child, spawn[0], NULL, NULL, (char * const *)spawn, environ);
    if (error) {
        NSLog(@"error with posix_spawnp");
        exit(1);
    }

    scrn = NULL;
    
    
    fp = fopen(fifo_path, "r");
    assert(fp);
#if 1
    bytes_read = getline(&scrn, &len, fp);
#else
    scrn=":0";
#endif
    fclose(fp);
    
    if (bytes_read && scrn[bytes_read - 1] == '\n')
        scrn[bytes_read - 1] = '\0';
    
    [XtoqApplication sharedApplication];
    controller = [[XtoqController alloc] init];
    [controller setScreen:scrn];
    [NSApp setDelegate: controller];
    [NSApp run];

    return 1;
}

@end
