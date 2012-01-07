/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_api.c
 */

#include "lpxcb_api.h"

/**
 * Return an lpxcb_window_t if there is damage, otherwise 0
 */
lpxcb_window_t *
lpxcb_check_window (xcb_connection_t *conn, xcb_window_t window)
{
    lpxcb_window_t *lpxcb_window;

    lpxcb_handle_events(conn);

    lpxcb_window = lpxcb_find_damaged(conn);

    while (lpxcb_window) {
        if (lpxcb_window->window == window) {
            lpxcb_update_window(lpxcb_window);
            return lpxcb_window;
        }
        lpxcb_window = lpxcb_window->next_damaged;
    }

    return lpxcb_window;
}
