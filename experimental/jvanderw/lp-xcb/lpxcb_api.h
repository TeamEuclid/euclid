/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb.h
 */

#include <xcb/xcb.h>


typedef struct
{
    xcb_connection_t *conn;
    xcb_window_t window;
} lpxcb_window;


void
reparent_existing_windows(xcb_connection_t *conn, xcb_window_t root);

lpxcb_window *
lpxcb_find_window (xcb_connection_t *conn, xcb_window_t window);

lpxcb_window *
lpxcb_add_window (xcb_connection_t *conn, xcb_window_t window);
