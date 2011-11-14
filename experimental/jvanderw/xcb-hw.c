/* Copyright (c) 2011 Jess VanDerwalker
 *
 * Test program to create a window using the XCB library.
 *
 * The majority of this is taken from the XCB tutorial at:
 *
 * http://www.x.org/releases/X11R7.6/doc/libxcb/tutorial/index.html
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <xcb/xcb.h>



int main (int argc, char **argv) {

    xcb_connection_t *connection; /* The connection to the X server */
    xcb_screen_t *screen;         /* The screen window will go into */
    xcb_drawable_t win;           /* The ID of the window we are going
                                   * to draw into */
    xcb_font_t font;            /* The font for the GC */
    xcb_gcontext_t gc;          /* ID of the graphical context */
    xcb_generic_event_t *evt;   /* */
    uint32_t mask = 0;          /* Bit mask used to set options */
    uint32_t values[2];         /* Array that holds values used by GC
                                 * as called for by the value set in
                                 * the mask */
    char fontname[] = "-adobe-courier-medium-o-normal--11-80-100-100-m-60-iso10646-1";

    /* Open the connection to the X server */
    connection = xcb_connect(NULL, NULL);
    /* We're just getting the data out of the first thing the iterator
     * points to */
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    /* Get a font for our window */
    font = xcb_generate_id(connection);
    xcb_open_font(connection, font, strlen(fontname), fontname);

    /* Create a graphic context for drawing */
    win = screen->root;
    gc = xcb_generate_id(connection);
    mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    values[0] = screen->black_pixel;
    values[1] = screen->white_pixel;
    values[2] = font;
    xcb_create_gc(connection, gc, win, mask, values);

    /* Done with te font */
    xcb_close_font(connection, font);
 

    /* Create a new window */
    win = xcb_generate_id(connection);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, win, screen->root,
                      0, 0, 400, 400, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, mask, values);

    /* Map the window to the screen and flush any pending messages */
    xcb_map_window(connection, win);
    xcb_flush(connection);

    /* Setup a loop to handle events. Note that this uses the blocking
     * style of event handling loop */
    while ((evt = xcb_wait_for_event(connection))) {
        switch (evt->response_type & ~0x80) {
        case XCB_EXPOSE: {
            xcb_expose_event_t *exevnt = (xcb_expose_event_t *)evt;

            printf("Window %ld exposed. Region to be redrawn at location (%d, %d), ",
                   exevnt->window, exevnt->x, exevnt->y);
            printf("with dimentions (%d, %d).\n", exevnt->width, exevnt->height);

            break;
        }
        case XCB_BUTTON_PRESS: {
            xcb_button_press_event_t *bpevnt = (xcb_button_press_event_t *)evt;
            printf("Button %d pressed in window %ld, at coordinates (%d, %d)\n",
                   bpevnt->detail, bpevnt->event, bpevnt->event_x, bpevnt->event_y);
            break;
        }
        case XCB_BUTTON_RELEASE: {
            xcb_button_release_event_t *brevnt = (xcb_button_release_event_t *)evt;
            printf("Button %d released in window %ld, at coordinates (%d, %d)\n",
                   brevnt->detail, brevnt->event, brevnt->event_x, brevnt->event_y);
            break;
        }
        case XCB_MOTION_NOTIFY: {
            xcb_motion_notify_event_t *mnevnt = (xcb_motion_notify_event_t *)evt;
            printf("Mouse moved in window %ld, at coordinates (%d, %d)\n",
                   mnevnt->event, mnevnt->event_x, mnevnt->event_y);
            break;
        }
        case XCB_KEY_PRESS: {
            xcb_key_press_event_t *kpevnt = (xcb_key_press_event_t *)evt;
            printf("Key pressed in window %ld\n", kpevnt->event);
            break;
        }
        default: {
            /* Ignore unknown events */
            printf("Unknown event: %d\n", evt->response_type);
            break;
        }
        }
        free(evt);              /* Need to free the event */
    }

    /* Close the connection */
    xcb_disconnect(connection);

    return 0;
}
