/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb is an attempt to recreate Keith Packards lightpipe using the
 * XCB library instead Xlib */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <xcb/xcb.h>
#include "lpxcb_api.h"
#include "lpxcb_table.h"
#include "lpxcb_util.h"

int
main (int argc, char **argv)
{
    xcb_connection_t *conn;
    xcb_connection_t *new_conn;
    int conn_screen;            /* Number of the screen we're be connected to */
    xcb_screen_t *root_screen;
    xcb_setup_t *setup;
    xcb_drawable_t root_window;
    xcb_drawable_t new_window = 0;
    lpxcb_window_t *lpxcb_window;
    uint8_t root_depth;
    xcb_query_pointer_cookie_t ptr_cookie;
    xcb_void_cookie_t void_cookie;
    xcb_get_geometry_reply_t *geom_reply;
    uint32_t mask;
    uint32_t values[1];
    xcb_generic_event_t *event;
    int error;

    /* Get the connection, root screen and root window - as well as
     * geometry of root window, its depth and the query pointer? */
    conn = xcb_connect(NULL, &conn_screen);
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;
    root_depth = root_screen->root_depth;
    ptr_cookie = xcb_query_pointer(conn, root_window);

    geom_reply = lpxcb_get_window_geometry(conn, root_window);
    if (geom_reply == NULL) {
        fprintf(stderr, "ERROR: Could not get geometry of the root window, exiting\n");
        return 1;
    }
    printf("For window id: %ld\n", (unsigned int)((uint32_t)root_window) );
    printf("x: %d, y: %d\n", geom_reply->x, geom_reply->y);
    printf("height: %d, width: %d\n", geom_reply->height, geom_reply->width);

    mask = XCB_CW_EVENT_MASK;
    values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
        XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_PROPERTY_CHANGE |
        XCB_EVENT_MASK_ENTER_WINDOW;

    void_cookie = xcb_change_window_attributes_checked(conn, root_window, mask, values);
    error = lpxcb_check_request(conn, void_cookie,
                                "Couldn't change window attibutes on root window");

    /* If we can't change attributes for root, create a "new" root */
    if (error) {
        printf("Creating new \"root\" window.\n");
        new_window = xcb_generate_id(conn);
        void_cookie = xcb_create_window_checked(conn, XCB_COPY_FROM_PARENT,
                                                new_window, root_window,
                                                0, 0, geom_reply->width,
                                                geom_reply->height,
                                                geom_reply->border_width,
                                                XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                                root_screen->root_visual, mask, values);
        error = lpxcb_check_request(conn, void_cookie,
                                    "Could not create new \"root\" window");
        if (error) {
            free(geom_reply);
            return 1;
        }
        /* Window created is our new root */
        root_window = new_window;
    }
    free(geom_reply);

    /* Add the window to our data structure */
    lpxcb_window = lpxcb_add_window(conn, root_window);

    /* Lets map the new window */
    /* xcb_map_window(conn, root_window); */
    xcb_flush(conn);

    /* Start the event loop before we create our window */
    while ((event = xcb_wait_for_event(conn))) {
        if (!new_window && xcb_wait_for_event(new_conn)) {
            lpxcb_window = lpxcb_check_window(conn, root_window);
            if (!new_window) {
                /* Right now, assume the same screen as the first connection */
                new_conn = xcb_connect(NULL, &conn_screen);
                new_window = xcb_generate_id(new_conn);
            }
        }
    }
    
    xcb_disconnect(conn);
    return 0;
}
