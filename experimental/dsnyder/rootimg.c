/* Copyright (c) 2012 Jess VanDerwalker
 *
 * rootimg.c
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


#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include "util.h"

/* Main driver */
int
main (int argc, char **argv)
{
    xcb_connection_t *conn;
    int conn_screen;
    xcb_screen_t *root_screen;
    xcb_drawable_t root_window;
    xcb_connection_t *conn_two;
    int conn_two_screen;
    xcb_screen_t *root_two_screen;
    xcb_drawable_t root_two_window;
    xcb_drawable_t window;

    uint32_t mask;
    uint32_t values[1];

    xcb_void_cookie_t cookie;

    xcb_get_geometry_reply_t *geom_reply;

    xcb_generic_event_t *event;

    image_data_t img_data;
    xcb_image_t *image;
    xcb_pixmap_t pixmap;
    xcb_gcontext_t gc;

    /* Check the first argument to see what display to connect to. If
       empty, then use default display. */
    if (argc > 1) {
      conn = xcb_connect(argv[1], &conn_screen);
    } else {
      conn = xcb_connect(NULL, &conn_screen);
    }
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;

    /* Get the geometry of the root window */
    geom_reply = GetWindowGeometry(conn, root_window);
    
    WriteWindowInfo(conn, root_window);
	WriteAllChildrenWindowInfo(conn, root_window);
    img_data = GetWindowImageData(conn, root_window);

	xcb_flush(conn);
    /* Get the image of the root window */
    image = xcb_image_get(conn,
                          root_window,
                          geom_reply->x,
                          geom_reply->y,
                          geom_reply->width,
                          geom_reply->height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    /* Set up the events the window will recognize */
    mask = XCB_CW_EVENT_MASK;
    values[0] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

    /* Create our new window on the default display. Make it half the size */
    conn_two = xcb_connect(NULL, &conn_two_screen);
    root_two_screen = xcb_aux_get_screen(conn_two, conn_two_screen);
    root_two_window = root_two_screen->root;
    window = xcb_generate_id(conn_two);
    cookie = xcb_create_window_checked(conn_two,
                                       XCB_COPY_FROM_PARENT,
                                       window,
                                       root_two_window,
                                       geom_reply->x,
                                       geom_reply->y,
                                       geom_reply->width / 2,
                                       geom_reply->height / 2,
                                       geom_reply->border_width,
                                       XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                       root_two_screen->root_visual,
                                       mask,
                                       values);
    if (RequestCheck(conn_two, cookie, "Falied to create new window")) {
        exit(1);
    }

    WriteWindowInfo(conn_two, window);
    /* Map the window and flush the connection so it draws to the screen */
    xcb_map_window(conn_two, window);
    xcb_flush(conn_two);
    WriteWindowInfo(conn_two, window);

    /* Create the pixmap and associate it with our new window. */
    pixmap = xcb_generate_id(conn_two);
    cookie = xcb_create_pixmap(conn_two,
                               geom_reply->depth,
                               pixmap,
                               window,
                               geom_reply->width,
                               geom_reply->height);
    if (RequestCheck(conn_two, cookie, "Failed to create pixmap")) {
        exit(1);
    }

    /* Put the root_window image into the pixmap. Note that a gc is
     * created, but I believe it is ignored. */
    gc = xcb_generate_id(conn_two);
    xcb_create_gc(conn_two, gc, window, 0, 0);
    cookie = xcb_image_put(conn_two,
                           pixmap,
                           gc,
                           image,
                           0,
                           0,
                           0);
    if (RequestCheck(conn_two, cookie, "Failed to put image into pixmap")) {
        exit(1);
    }

    /* Copy the pixmap into the new window */
    cookie = xcb_copy_area(conn_two,
                           pixmap,
                           window,
                           gc,
                           0,
                           0,
                           0,
                           0,
                           geom_reply->width / 2,
                           geom_reply->height / 2);
    if (RequestCheck(conn_two, cookie, "Failed to put image into pixmap")) {
        exit(1);
    }
                           
    xcb_flush(conn_two);
    WriteWindowInfo(conn_two, window);

    /* Enter infinte loop so the window stays open */
    while (1) {
    }

    /* Never get here, but if we could, would still want to clean up memory */
    free(geom_reply);
    xcb_disconnect(conn);
    xcb_disconnect(conn_two);

    return 0;
}
