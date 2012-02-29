/*Copyright (C) 2012 Ben Huddle, Braden Wooley, David Snyder
 
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

#import "XtoqWindow.h"

@implementation XtoqWindow



-(id) initWithContentRect:(NSRect)contentRect 
                styleMask:(NSUInteger)aStyle 
                  backing:(NSBackingStoreType)bufferingType 
                    defer:(BOOL)flag {
    
    notificationCenter = [NSNotificationCenter defaultCenter];
    
    // Hack job, probably -- David
    [[NSNotificationCenter defaultCenter] addObserver: self 
                                             selector: @selector(windowDidBecomeKey:) 
                                                 name: @"NSWindowDidBecomeKeyNotification" 
                                               object: self];
    // End Hack job -- David
    
    XtoqWindow *result = [super initWithContentRect:contentRect
                                        styleMask:aStyle
                                          backing:bufferingType
                                            defer:flag];
    return result;
}

-(void) makeKeyAndOrderFront:(id)sender {
    [super makeKeyAndOrderFront:sender];
}

-(void) setContext:(xtoq_context_t *)aContext {
    winContext = aContext;
}

-(xtoq_context_t *) getContext {
    return winContext;
}

- (BOOL) windowShouldClose:(id)sender {    
    // send notification to controller to close the window
    XtoqWindow * aWindow = self;
    NSDictionary * dictionary = [NSDictionary dictionaryWithObject:aWindow 
                                                            forKey:@"1"];    
    [notificationCenter postNotificationName:@"XTOQdestroyTheWindow" 
                                      object:self
                                    userInfo:dictionary];  
    
    // keep window from closing till server tells it to
    return NO;
}

-(void)windowDidBecomeKey:(NSNotification *)note {
    
    xtoq_set_input_focus(winContext);
    xtoq_set_window_to_top(winContext);
}

@end
