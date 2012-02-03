/* Copyright (c) 2012 David Snyder
 *
 * rootimg_api.h
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
//#include "context_list.h"
#include "util.h"

#define XTOQ_DAMAGE 0

typedef struct xtoq_context_t {
    xcb_connection_t *conn;
    xcb_drawable_t window;
} xtoq_context_t;

typedef struct xtoq_event_t {
    xtoq_context_t context;
    int event_type;
} xtoq_event_t;


/**
 * Sets up the connection and grabs the root window from the specified screen
 * @param screen The screen that we wish to connect to
 */
xtoq_context_t
xtoq_init(char *screen);

xcb_image_t *
xtoq_get_image(xtoq_context_t context);

/*
xtoq_event_t
xtoq_wait_for_event(xtoq_context_t context);
*/

int
dummy_xtoq_get_image(xtoq_context_t context);

xtoq_event_t
dummy_xtoq_wait_for_event(xtoq_context_t context);




#endif // _XTOQ_H_
