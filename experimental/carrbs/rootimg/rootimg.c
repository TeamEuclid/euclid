/* Copyright (c) 2012 Jess VanDerwalker
 *
 * rootimg.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>


/* Utility function to ease checking for request errors */
int
RequestCheck (xcb_connection_t *conn, xcb_void_cookie_t cookie,
                     char *msg)
{
    xcb_generic_error_t *error;

    error = xcb_request_check(conn, cookie);
    if (error) {
        if (msg) {
            fprintf(stderr, "ERROR: ");
            //fprintf(stderr, msg);
            fprintf(stderr, "\nError code: %d\n", error->error_code);
        }
        return error->error_code;
    }
    return 0;
}

/* Utility function to get the geometry of the given window. Memory
 * allocated for the xcb_get_geometry_reply_t must be freed by the
 * caller */
xcb_get_geometry_reply_t *
GetWindowGeometry (xcb_connection_t *conn, xcb_window_t window)
{
    xcb_get_geometry_cookie_t cookie;
    cookie = xcb_get_geometry(conn, window);
    return xcb_get_geometry_reply(conn, cookie, NULL);
}

/* Main driver */
int
main (int argc, char **argv)
{
    xcb_connection_t *conn;
    int conn_screen;
    xcb_screen_t *root_screen;
    xcb_drawable_t root_window;
    xcb_drawable_t window;

    uint32_t mask;
    uint32_t values[1];

    xcb_void_cookie_t cookie;

    xcb_get_geometry_reply_t *geom_reply;

    xcb_image_t *image;
    xcb_pixmap_t pixmap;
    xcb_gcontext_t gc;

    /* By using NULL as first argument, the value of $DISPLAY is used */
    conn = xcb_connect(NULL, &conn_screen);
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;

    /* Get the geometry of the root window */
    geom_reply = GetWindowGeometry(conn, root_window);

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

    /* Create our new window. Make it half the size */
    window = xcb_generate_id(conn);
    cookie = xcb_create_window_checked(conn,
                                       XCB_COPY_FROM_PARENT,
                                       window,
                                       root_window,
                                       geom_reply->x / 2,
                                       geom_reply->y / 2,
                                       geom_reply->width / 2,
                                       geom_reply->height / 2,
                                       geom_reply->border_width,
                                       XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                       root_screen->root_visual,
                                       mask,
                                       values);
    if (RequestCheck(conn, cookie, "Falied to create new window")) {
        exit(1);
    }

    /* Map the window and flush the connection so it draws to the screen */
    xcb_map_window(conn, window);
    xcb_flush(conn);

    /* Create the pixmap and associate it with our new window. */
    pixmap = xcb_generate_id(conn);
    cookie = xcb_create_pixmap(conn,
                               geom_reply->depth,
                               pixmap,
                               window,
                               geom_reply->width,
                               geom_reply->height);
    if (RequestCheck(conn, cookie, "Failed to create pixmap")) {
        exit(1);
    }

    /* Put the root_window image into the pixmap. Note that a gc is
     * created, but I believe it is ignored. */
    gc = xcb_generate_id(conn);
    xcb_create_gc(conn, gc, window, 0, 0);
    cookie = xcb_image_put(conn,
                           pixmap,
                           gc,
                           image,
                           0,
                           0,
                           0);
    if (RequestCheck(conn, cookie, "Failed to put image into pixmap")) {
        exit(1);
    }

    /* Copy the pixmap into the new window */
    cookie = xcb_copy_area(conn,
                           pixmap,
                           window,
                           gc,
                           0,
                           0,
                           0,
                           0,
                           geom_reply->width,
                           geom_reply->height);
    if (RequestCheck(conn, cookie, "Failed to put image into pixmap")) {
        exit(1);
    }

    xcb_flush(conn);
    while(1);
/*
     //Enter infinte loop so the window stays open 
    while ((event = xcb_wait_for_event(conn))) {
        switch (event->response_type & ~0x80) {
        case XCB_KEY_PRESS: {
            kpevent = (xcb_key_press_event_t *) event;
            printf("A key was pressed\n");
        }
        default: {
            break;
        }
        }
    }
*/
    /* Never get here, but if we could, would still want to clean up memory */
    free(geom_reply);
    xcb_disconnect(conn);

    return 0;
}
