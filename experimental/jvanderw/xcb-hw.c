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
#include <xcb/xcb_image.h>


/* Print out some information about the given window */
void print_win_info(xcb_connection_t *conn, xcb_drawable_t win) {

    xcb_get_geometry_reply_t *geom; /* Property reply structure */
    xcb_get_geometry_cookie_t geomcookie; /* Response cookies for getting geometry */
    xcb_generic_error_t *error;           /* Error reporting structure */

    /* Get the window geometry */
    geomcookie = xcb_get_geometry(conn, win);
    geom = xcb_get_geometry_reply(conn, geomcookie, &error);
    if (error) {
        fprintf(stderr, "ERROR: Failed to get geometry of the window: %d\n",
                error->error_code);
    }
    
    printf("For window id: %ld\n", (unsigned int)((uint32_t)win) );
    printf("x: %d, y: %d\n", geom->x, geom->y);
    free(geom);

}

/* Print out information about the existing windows attached to our
 * root. Most of this code is taken from src/manage.c from the i3 code
 * by Michael Stapelberg */
void print_all_win_info(xcb_connection_t *conn, xcb_window_t root) {

    xcb_query_tree_reply_t *reply;
    xcb_query_tree_cookie_t tree_cookie;
    xcb_window_t *children;     /* The children of the given root */
    xcb_generic_error_t *error;
    xcb_get_image_reply_t *image;
    xcb_get_image_cookie_t image_cookie;
    int len;
    int i;

    tree_cookie = xcb_query_tree(conn, root);
    reply = xcb_query_tree_reply(conn, tree_cookie, &error);
    if (error) {
        fprintf(stderr, "ERROR: Failed to get query tree: %d\n",
                error->error_code);
        return;
    }
    /* Get the number of children */
    len = xcb_query_tree_children_length(reply);
    children = xcb_query_tree_children(reply);

    /* Iterate thorough all the children and get their pixmap (hopefully) */
    for (i = 0; i < len; i++) {
        print_win_info(conn, children[i]);
        image_cookie = xcb_get_image(conn, XCB_IMAGE_FORMAT_XY_PIXMAP,
                                     children[i], 0 , 0, 200, 200, -1);
        image = xcb_get_image_reply(conn, image_cookie, &error);
        if (error) {
            fprintf(stderr, "ERROR: Failed to get image from drawable: %d\n",
                error->error_code);
        }
    }
    
    /* Free the stuff allocated by XCB */
    free(reply);
}

/* Get the first valid pixmap from the children given, and "put" in
 * the given drawable */
void assign_pixmap(xcb_connection_t *conn, xcb_drawable_t drawable,
                   xcb_drawable_t win, xcb_gcontext_t gc) {

    xcb_query_tree_reply_t *reply;
    xcb_query_tree_cookie_t tree_cookie;
    xcb_window_t *children;     /* The children of the given win */
    xcb_generic_error_t *error;
    xcb_image_t *image;
    xcb_void_cookie_t imcookie;
    int len;
    int i;

    tree_cookie = xcb_query_tree(conn, win);
    reply = xcb_query_tree_reply(conn, tree_cookie, &error);
    if (error) {
        fprintf(stderr, "ERROR: Failed to get query tree: %d\n",
                error->error_code);
        return;
    }
    /* Get the number of children */
    len = xcb_query_tree_children_length(reply);
    children = xcb_query_tree_children(reply);

    /* Iterate thorough all the children and get their pixmap (hopefully) */
    for (i = 0; i < len; i++) {
        print_win_info(conn, children[i]);
        image = xcb_image_get(conn, children[i], 0, 0, 200, 200, ~0,
                              XCB_IMAGE_FORMAT_Z_PIXMAP);

        /* if (image) { */
        /*     if (*(image->data) == '\0') { */
        /*         printf("No data in image\n"); */
        /*         continue; */
        /*     } */

        /*     imcookie = xcb_image_put(conn, drawable, gc, image, 0, 0, 0); */

        /*     error = xcb_request_check(conn, imcookie); */
        /*     if (error) { */
        /*         fprintf(stderr, "ERROR: Failed to put pixmap: %d\n", */
        /*                 error->error_code); */
        /*     } */
        /*     free(reply); */
        /*     return;             /\* Bail after assigning first one *\/ */
        /* } */

        /* Crazy testing stuff here */
        /* create backing pixmap */
        xcb_pixmap_t pmap;

        pmap = xcb_generate_id(conn);
        xcb_create_pixmap(conn, 24, pmap, drawable, 200, 200);



        imcookie = xcb_copy_area(conn, children[i], pmap, gc, 0, 0, 0, 0,
                                 200, 200);
        error = xcb_request_check(conn, imcookie);
        if (error) {
            fprintf(stderr, "ERROR: Failed to copy pixmap: %d\n",
                    error->error_code);
        }
        free(reply);
        return;
    }
    
    /* Free the stuff allocated by XCB */
    free(reply);
}

/* Main program loop */
int main (int argc, char **argv) {

    xcb_connection_t *conn; /* The connection to the X server */
    xcb_screen_t *screen;         /* The screen window will go into */
    xcb_drawable_t win;           /* The ID of the window we are going
                                   * to draw into */
    xcb_drawable_t winchild;    /* Child window */
    xcb_font_t font;            /* The font for the GC */
    xcb_gcontext_t gc;          /* ID of the graphical context */
    xcb_generic_event_t *evt;
    uint32_t mask = 0;          /* Bit mask used to set options */
    uint32_t values[2];         /* Array that holds values used by GC
                                 * as called for by the value set in
                                 * the mask */
    char fontname[] = "-adobe-courier-medium-o-normal--11-80-100-100-m-60-iso10646-1";
    xcb_void_cookie_t cookie_conf; /* Response cookie for call to configure */
    xcb_generic_error_t *error;    /* Struct for error information */
    xcb_void_cookie_t cookie_win; /* Response cookie for call to window create */   

    /* Open the connection to the X server */
    conn = xcb_connect(NULL, NULL);
    /* We're just getting the data out of the first thing the iterator
     * points to */
    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

    /* Get a font for our window */
    font = xcb_generate_id(conn);
    xcb_open_font(conn, font, strlen(fontname), fontname);

    /* Create a graphic context for drawing */
    win = screen->root;
    gc = xcb_generate_id(conn);
    mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
    values[0] = screen->black_pixel;
    values[1] = screen->white_pixel;
    values[2] = font;
    xcb_create_gc(conn, gc, win, mask, values);

    /* Done with te font */
    xcb_close_font(conn, font);
 

    /* Create a new window */
    win = xcb_generate_id(conn);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;

    xcb_create_window(conn, XCB_COPY_FROM_PARENT, win, screen->root,
                      0, 0, 400, 400, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, mask, values);

    /* Map the window to the screen and flush any pending messages */
    xcb_map_window(conn, win);
    xcb_flush(conn);
    print_win_info(conn, win);

    /* Create a second window an make it child of the previous one */
    winchild = xcb_generate_id(conn);
    values[0] = screen->black_pixel;
    cookie_win = xcb_create_window_checked(conn, XCB_COPY_FROM_PARENT, winchild,
                                           win,
                                           20, 20, 100, 100, 10,
                                           XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                           screen->root_visual, mask, values);
    error = xcb_request_check(conn, cookie_win);
    if (error) {
        fprintf(stderr, "ERROR: Failed to create window: %d\n",
                error->error_code);
    }
    /* Assign an existing image to the window */
    assign_pixmap(conn, win, screen->root, gc);

    xcb_map_window(conn, win);
    xcb_map_window(conn, winchild);
    xcb_flush(conn);

    /* Print out information on all the windows based on this root */
    /* print_all_win_info(conn, screen->root); */

    /* Setup a loop to handle events. Note that this uses the blocking
     * style of event handling loop */
    while ((evt = xcb_wait_for_event(conn))) {
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

            /* Resize the window on the button press */
            values[0] = 600;
            values[1] = 600;
            cookie_conf = xcb_configure_window_checked(conn, win,
                                                       XCB_CONFIG_WINDOW_X |
                                                       XCB_CONFIG_WINDOW_Y,
                                                       values);
            /* Check the cookies for errors */
            error = xcb_request_check(conn, cookie_conf);
            if (error) {
                fprintf(stderr, "ERROR: Failed to reconfigure the window: %d\n",
                        error->error_code);
            }
            xcb_map_window(conn, win);

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
            /* printf("Mouse moved in window %ld, at coordinates (%d, %d)\n", */
            /*        mnevnt->event, mnevnt->event_x, mnevnt->event_y); */
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
    xcb_disconnect(conn);

    return 0;
}
