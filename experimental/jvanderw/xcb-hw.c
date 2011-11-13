/* Copyright (c) 2011 Jess VanDerwalker
 *
 * Test program to create a window using the XCB library.
 */

#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>

int main (int argc, char **argv) {

    xcb_connection_t *connection; /* The connection to the X server */
    xcb_screen_t *screen;         /* The screen window will go into */
    xcb_drawable_t win;           /* The ID of the window we are going
                                   * to draw into */
    xcb_gcontext_t gc;          /* ID of the graphical context */
    xcb_generic_event_t *evt;   /* */
    uint32_t mask;              /* Bit mask for GC options */
    uint32_t values[2];         /* Array that holds values used by GC
                                 * as called for by the value set in
                                 * the mask */

    /* Some shapes */
    xcb_point_t polyline[] = {
        {50, 10}, {5, 20}, {25, -20}, {10, 10}
    };

    xcb_rectangle_t rectangle[] = {
        {10, 50, 40, 20},
        {80, 50, 10, 40}
    };

    /* Open the connection to the X server */
    connection = xcb_connect(NULL, NULL);
    /* We're just getting the data out of the first thing the iterator
     * points to */
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    /* Create a graphic context for drawing */
    win = screen->root;
    gc = xcb_generate_id(connection);
    mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    values[0] = screen->black_pixel;
    values[1] = 0;
    xcb_create_gc(connection, gc, win, mask, values);

    win = xcb_generate_id(connection);

    /* Create a new window */
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE;
    xcb_create_window(connection, XCB_COPY_FROM_PARENT, win, screen->root,
                      0, 0, 400, 400, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, mask, values);

    /* Map the window to the screen and flush any pending messages */
    xcb_map_window(connection, win);
    xcb_flush(connection);

    /* Setup a loop to handle events */
    while ((evt = xcb_wait_for_event(connection))) {
        switch (evt->response_type & ~0x80) {
        case XCB_EXPOSE: {
            /* Draw the polyline and the rectangle */
            xcb_poly_line(connection, XCB_COORD_MODE_PREVIOUS, win, gc,
                          4, polyline);
            xcb_poly_rectangle(connection, win, gc, 2, rectangle);
            xcb_flush(connection);
            break;
        }
        default: {
            /* Ignore unknown events */
            break;
        }
        }
        free(evt);              /* Need to free the event */
    }
    pause();

    /* Close the connection */
    xcb_disconnect(connection);

    return 0;
}
