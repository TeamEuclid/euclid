/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_util.c
 */

#include <string.h>
#include <stdio.h>
#include "lpxcb_util.h"

int
lpxcb_check_request (xcb_connection_t *conn, xcb_void_cookie_t cookie,
                     char *msg)
{
    xcb_generic_error_t *error;

    error = xcb_request_check(conn, cookie);
    if (error) {
        if (msg) {
            fprintf(stderr, "ERROR: ");
            fprintf(stderr, msg);
            fprintf(stderr, "\nError code: %d\n", error->error_code);
        }
        return error->error_code;
    }
    return 0;
}

xcb_get_geometry_reply_t *
lpxcb_get_window_geometry (xcb_connection_t *conn, xcb_window_t window)
{
    xcb_get_geometry_cookie_t cookie;
    cookie = xcb_get_geometry(conn, window);
    return xcb_get_geometry_reply(conn, cookie, NULL);
}

void
lpxcb_set_window_dimensions (xcb_connection_t *conn, xcb_window_t window,
                             uint16_t x, uint16_t y,
                             uint16_t height, uint16_t width,
                             uint16_t border_width)
{
    uint32_t values[] = {x , y, height, width, border_width};
    xcb_void_cookie_t cookie;

    cookie = xcb_configure_window_checked(conn, window,
                                          XCB_CONFIG_WINDOW_X |
                                          XCB_CONFIG_WINDOW_Y |
                                          XCB_CONFIG_WINDOW_WIDTH | 
                                          XCB_CONFIG_WINDOW_HEIGHT,
                                          values);
    lpxcb_check_request(conn, cookie, "Failed to set new window position and dimentions");
}

xcb_get_window_attributes_reply_t *
lpxcb_get_window_attrs (xcb_connection_t *conn, xcb_window_t window)
{
    xcb_get_window_attributes_cookie_t cookie;
    xcb_generic_error *error;
    xcb_get_window_attributes_reply_t *reply;
    
    cookie = xcb_get_window_attributes(conn, window);
    reply = xcb_get_window_attributes_reply(conn, cookie, &error);
    if (error) {
        fprintf(stderr, "ERROR: Failed to get window attributes: %d\n",
                error->error);
        return NULL;
    }
    return reply;
}
