
/*Copyright (C) 2012 Aaron Skomra 
 
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

/** 
 *  AppController.m
 *  xtoq
 *
 *  This is the controller for the Popup to retreive the display number
 *  from the user.
 */

#import "AppController.h"
#import "DisplayNumberController.h"
#import "xtoq.h"

@implementation AppController

 
- (IBAction)showDisplayChooser
{
   if (!displayNumberController){
        displayNumberController = [[DisplayNumberController alloc] init];
    }
    NSLog(@"showing %@", displayNumberController);
    [displayNumberController showWindow:self];
    NSLog(@"opened %@", displayNumberController);	
}

/**
 *
 * Following the steps from David's email, I produced this which does some
 * weird stuff.
 *
 */
- (void)wait_for_xtoq_event {
    /*xtoq_context_t xqcontxt;
    xtoq_event_t xqevent;
    char *screen;// = "1";
    
    screen = getenv("DISPLAY"); //change with dialog display number later
    
    xqcontxt = xtoq_init(screen);
    xqevent = dummy_xtoq_wait_for_event(xqcontxt);
    
    if (xqevent.event_type == XTOQ_DAMAGE) {
        dummy_xtoq_get_image(xqevent.context);
    }
    else { NSLog(@"HEy I'm Not damage!"); }*/
    
}
@end
