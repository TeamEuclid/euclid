/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_event.c
 */

#include "lpxcb_event.h"

void
lpxcb_process_event(xcb_connection_t *conn)
{
    xcb_generic_event_t *event;

    while ((event = xcb_poll_for_event(conn, 0))) {
        /* Handle the events here */
    }
}

