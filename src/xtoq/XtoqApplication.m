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

- (void) sendEvent:(NSEvent *)e {
    notificationCenter = [NSNotificationCenter defaultCenter];
    NSMutableDictionary *InfoDict;
    NSPoint ns_location = [e locationInWindow];
    NSWindow *ns_window = [e window];
    NSNumber *xNum, *yNum;
	BOOL for_appkit;
	BOOL for_x;

    /* By default pass down the responder chain and to X. */
    for_appkit = YES;
    for_x = YES;
    
    switch ([e type]) {
        case NSLeftMouseDown: case NSRightMouseDown: case NSOtherMouseDown:
        case NSLeftMouseUp: case NSRightMouseUp: case NSOtherMouseUp: {
            /* We want to force sending to appkit if we're over the menu bar */
            int buttonInt = [self eventToButtonInt:e];
            NSNumber *buttonNumber = [[NSNumber alloc] initWithInt:buttonInt];
            
		    NSPoint NSlocation = [e locationInWindow];
			NSWindow *window = [e window];
			NSRect NSframe, NSvisibleFrame;
			CGRect CGframe, CGvisibleFrame;
			CGPoint CGlocation;
                
			if (window != nil)	{
			  NSRect frame = [window frame];
			  NSlocation.x += frame.origin.x;
			  NSlocation.y += frame.origin.y;
			}
                
			NSframe = [[NSScreen mainScreen] frame];
			NSvisibleFrame = [[NSScreen mainScreen] visibleFrame];
                
			CGframe = CGRectMake(NSframe.origin.x, NSframe.origin.y,
								 NSframe.size.width, NSframe.size.height);
			CGvisibleFrame = CGRectMake(NSvisibleFrame.origin.x,
										NSvisibleFrame.origin.y,
										NSvisibleFrame.size.width,
										NSvisibleFrame.size.height);
			CGlocation = CGPointMake(NSlocation.x, NSlocation.y);
                
			if(CGRectContainsPoint(CGframe, CGlocation) &&
			   !CGRectContainsPoint(CGvisibleFrame, CGlocation))
			  for_appkit = YES;
            
			if (for_x) {
			    if  ([e type] == NSLeftMouseDown 
					 || [e type] == NSRightMouseDown
					 || [e type] == NSOtherMouseDown) {
                    
    			    CGFloat f = 100;
					NSNumber *n = [[NSNumber alloc] initWithFloat:f];
					NSMutableDictionary *threeInfoDict = [[NSMutableDictionary alloc] initWithCapacity:3];
					[threeInfoDict setObject:e forKey:@"1"];
					[threeInfoDict setObject:n forKey:@"2"];
                    [threeInfoDict setObject:buttonNumber forKey:@"3"];
                    
					[notificationCenter postNotificationName:@"XTOQmouseButtonDownEvent" 
					                                  object:self 
            										userInfo:threeInfoDict];
                } else {
				    CGFloat f = 100;
				    NSNumber *n = [[NSNumber alloc] initWithFloat:f];
				    NSMutableDictionary *threeInfoDict = [[NSMutableDictionary alloc] initWithCapacity:3];
				    [threeInfoDict setObject:e forKey:@"1"];
					[threeInfoDict setObject:n forKey:@"2"];
                    [threeInfoDict setObject:buttonNumber forKey:@"3"];
					[notificationCenter postNotificationName:@"XTOQmouseButtonReleaseEvent" 
										              object:self 
									                userInfo:threeInfoDict];
                }
			}
            break;
        }
        case NSMouseMoved: {        
            if (ns_window != nil) {
                NSRect frame = [ns_window frame];
                ns_location.x += frame.origin.x;
                ns_location.y += frame.origin.y;
            }
            
            xNum = [[NSNumber alloc] initWithFloat:ns_location.x];
            yNum = [[NSNumber alloc] initWithFloat:ns_location.y];
            
            InfoDict = [[NSMutableDictionary alloc] initWithCapacity:3];
            [InfoDict setObject:e forKey:@"1"];
            [InfoDict setObject:xNum forKey:@"2"];
            [InfoDict setObject:yNum forKey:@"3"];
            
            [notificationCenter postNotificationName:@"MouseMovedEvent" 
                                              object:self 
                                            userInfo:InfoDict];
            break;
        }
        case NSKeyDown: case NSKeyUp: {
            
            static BOOL do_swallow = NO;
            static int swallow_keycode;
            
            if([e type] == NSKeyDown) {
                /* Before that though, see if there are any global
                 * shortcuts bound to it. */
                
                // Zero takes the place of XQuartz's
                // darwinAppKitModMask
                if(0 & [e modifierFlags]) {
                    /* Override to force sending to Appkit */
                    swallow_keycode = [e keyCode];
                    do_swallow = YES;
                    for_x = NO;
                } else if ([[self mainMenu] performKeyEquivalent:e]) {
                    swallow_keycode = [e keyCode];
                    do_swallow = YES;
                    for_appkit = NO;
                    for_x = NO;
                } else {
                    /* No kit window is focused, so send it to X. */
                    for_appkit = NO;
                }
                if (for_x) {
                    NSDictionary * dictionary = [NSDictionary dictionaryWithObject:e 
                                                                            forKey:@"1"];
                    [notificationCenter postNotificationName:@"XTOQviewKeyDownEvent" 
                                                      object:self 
                                                    userInfo:dictionary];
                }
            } else { /* KeyUp */
                /* If we saw a key equivalent on the down, don't pass
                 * the up through to X. */
                if (do_swallow && [e keyCode] == swallow_keycode) {
                    do_swallow = NO;
                    for_x = NO;
                }
                if (for_x) {
                    NSDictionary * dictionary = [NSDictionary dictionaryWithObject:e 
                                                                            forKey:@"1"];
                    [notificationCenter postNotificationName:@"XTOQviewKeyUpEvent" 
                                                      object:self 
                                                    userInfo:dictionary];
                }
            }
            break;
        }
        default:
		    break;
        }
            if (for_appkit) {
                [super sendEvent:e];
            }
}

- (int) eventToButtonInt:(NSEvent *)e {
    if ([e type] == NSLeftMouseDown ||
        [e type] == NSLeftMouseUp) {
        return 1;
    }
    else if ([e type] == NSRightMouseDown ||
        [e type] == NSRightMouseUp ) {
        return 3;
    }    
    else if ([e type] == NSOtherMouseDown ||
             [e type] == NSOtherMouseUp ) {
        return 2;
    }
    else return -1;
}

@end
