/* Copyright (c) 2012 David Snyder
 *
 * xtoq.h
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


#ifndef _XTOQ_H_
#define _XTOQ_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include <xcb/damage.h>
#include <xcb/xtest.h>
#include <xcb/xfixes.h>
#include "data.h"
#include <xcb/xcb_keysyms.h> //aaron
#include "xtoq_internal.h"

// TODO: Remove this once we get the context_list in place. Right
// now this is here for testing purposes to give an easy way to get the
// root window's context in any function
extern xtoq_context_t *root_context;

/**
 * Sets up the connection and grabs the root window from the specified screen
 * @param screen The screen that we wish to connect to
 */
xtoq_context_t *
xtoq_init(char *screen);

xcb_image_t *
xtoq_get_image(xtoq_context_t *context);

/**
 * free the memory used by an xtoq_image_t created 
 * during a call to test_xtoq_image_create
 */
void 
xtoq_image_destroy(xtoq_image_t * xtoq_image);

/**
 * Free the image returned by xtoq_get_image
 * @param img The xcb_image_t that needs to
 * be freed
 */

void
xtoq_free_image(xcb_image_t *img);

/**
 * Starts the event loop and listens on the connection specified in
 * the given xtoq_context_t. Uses callback as the function to call
 * when an event of interest is received. Callback must be able to
 * take an xtoq_event_t as its one and only parameter.
 * @param root_context The context containing the connection to listen
 * for events on.
 * @param callback The function to call when an event of interest is
 * received.
 * @return Uses the return value of the call to pthread_create as 
 * the return value.
 */
int
xtoq_start_event_loop (xtoq_context_t *root_context, void *callback);

/**
 * Testing function
 * @param context xtoq_context_t 
 * @param window The window that the key press was made in.
 * @param keyCode The key pressed.
 */
void
dummy_xtoq_key_press (xtoq_context_t *context, int window, uint8_t code);

/**
 * Testing function
 * @param context xtoq_context_t 
 * @param x - x coordinate
 * @param y - y coordinate
 * @param window The window that the key press was made in.
 */
void
dummy_xtoq_button_down (xtoq_context_t *context, long x, long y, int window, int button);

xtoq_image_t *
test_xtoq_get_image(xtoq_context_t * context);
#endif // _XTOQ_H_
