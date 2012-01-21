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

#ifndef _ROOTIMG_API_C_
#define _ROOTIMG_API_C_

#include "rootimg_api.h"


context_t
init(int screen) {
    xcb_connection_t *conn;
    int conn_screen;
    xcb_screen_t *root_screen;
    xcb_drawable_t root_window;
    xcb_drawable_t window; // Is this one needed here?
    xcb_void_cookie_t cookie;
    
    xcb_get_geometry_reply_t *geom_reply;
    
    xcb_generic_event_t *event;
    
    xcb_gcontext_t gc;
 
    conn = xcb_connect(screen, &conn_screen);
    
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;
    
    /* Get the geometry of the root window */
    geom_reply = GetWindowGeometry(conn, root_window);
    
    WriteWindowInfo(conn, root_window);
	WriteAllChildrenWindowInfo(conn, root_window);
    
	xcb_flush(conn);
    
    context_t init_reply;
    init_reply.conn = conn;
    init_reply.window = root_window;
    
    return init_reply;
}

int
get_image(context_t context) {
    
    image_data_t img_data;
    xcb_image_t *image;
    
    image = xcb_image_get(context.conn,
                          context.window,
                          geom_reply->x,
                          geom_reply->y,
                          geom_reply->width,
                          geom_reply->height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    /* Set up the events the window will recognize */
    mask = XCB_CW_EVENT_MASK;
    values[0] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
    // ...
}

#endif _ROOTIMG_API_C_