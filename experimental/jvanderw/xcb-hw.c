/* Copyright (c) 2011 Jess VanDerwalker
 *
 * Test program to create a window using the XCB library.
 */

#include <unistd.h>
#include <xcb/xcb.h>

int main (int argc, char **argv) {

    xcb_connection_t *connection; /* The connection to the X server */
    xcb_screen_t *screen;         /* The screen window will go into */
    xcb_drawable_t win;           /* The ID of the window we are going
                                   * to draw into */
    xcb_gcontext_t gc;          /* ID of the graphical context */
    uint32_t mask;              /* Bit mask for GC options */
    uint32_t value[1];          /* ??? */

    /* Open the connection to the X server */
    connection = xcb_connect(NULL, NULL);
    /* We're just getting the data out of the first thing the iterator
     * points to */
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    /* Create a graphic context for drawing */
    win = screen->root;
    gc = xcb_generate_id(connection);
    mask = XCB_GC_FOREGROUND;
    value[0] = screen->black_pixel;
    xcb_create_gc(connection, gc, win, mask, value);

    win = xcb_generate_id(connection);

    /* Create a new window */
    xcb_create_window(connection, XCB_COPY_FROM_PARENT, win, screen->root,
                      0, 0, 400, 400, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, 0, NULL);

    /* Map the window to the screen and flush any pending messages */
    xcb_map_window(connection, win);
    xcb_flush(connection);
    pause();

    /* Close the connection */
    xcb_disconnect(connection);

    return 0;
}
