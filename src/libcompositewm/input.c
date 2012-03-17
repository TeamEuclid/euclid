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

//
//  input.c
//  xtoq-xcode

#include <stdio.h>
#include "xtoq.h"

void
xtoq_key_press (xtoq_context_t *context, int window, uint8_t code)
{
    xcb_window_t none = { XCB_NONE };
    
    xcb_test_fake_input( context->conn, XCB_KEY_PRESS, code, 
						 XCB_CURRENT_TIME, none, 0, 0, 1 );  
    
    xcb_flush(context->conn);
    printf("xtoq.c received key down - uint8_t '%i', from Mac window #%i to context.window %u\n", code,  window, context->window);
}

void
xtoq_key_release (xtoq_context_t *context, int window, uint8_t code)
{
    xcb_window_t none = { XCB_NONE };
    
    xcb_test_fake_input( context->conn, XCB_KEY_RELEASE, code, 
                        XCB_CURRENT_TIME, none,0 ,0 , 1 );
    
    xcb_flush(context->conn);
    printf("xtoq.c received key release- uint8_t '%i', from Mac window #%i to context.window %u\n", code,  window, context->window);
}

void
xtoq_button_press (xtoq_context_t *context, long x, long y, int window, int button)
{
    //xcb_window_t none = { XCB_NONE };
    xcb_test_fake_input (context->conn, XCB_BUTTON_PRESS, 1, XCB_CURRENT_TIME,
                         context->window, 0, 0, 0);
	xcb_flush(context->conn);
    printf("button down received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
}

void
xtoq_button_release (xtoq_context_t *context, long x, long y, int window, int button)
{
    xcb_test_fake_input (context->conn, XCB_BUTTON_RELEASE, 1, XCB_CURRENT_TIME,
                         context->window, 0, 0, 0);
	xcb_flush(context->conn);
    printf("button release received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
}

void
xtoq_right_button_press (xtoq_context_t *context, long x, long y, int window, int button)
{
/*    //xcb_window_t none = { XCB_NONE };
    xcb_test_fake_input (context->conn, XCB_BUTTON_PRESS, 1, XCB_CURRENT_TIME,
                         context->window, 0, 0, 0);
        xcb_flush(context->conn);
    printf("button down received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
*/}

void
xtoq_right_button_release (xtoq_context_t *context, long x, long y, int window, int button)
{
   /* xcb_test_fake_input (context->conn, XCB_BUTTON_RELEASE, 1, XCB_CURRENT_TIME,
                         context->window, 0, 0, 0);
        xcb_flush(context->conn);
    printf("button release received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
*/}

void
xtoq_mouse_motion (xtoq_context_t *context, long x, long y, int window, int button)
{
    xcb_test_fake_input (context->conn, XCB_MOTION_NOTIFY, 0, XCB_CURRENT_TIME,
                         root_context->window//root_context->window//none//context->parent
                         ,x, y, 0);
	xcb_flush(context->conn);
    //printf("mouse motion received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
}


