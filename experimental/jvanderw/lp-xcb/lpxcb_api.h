/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb.h
 */

#ifndef _LPXCB_API_H_
#define _LPXCB_API_H_

#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/damage.h>
#include <xcb/composite.h>
#include <xcb/xfixes.h>


/* Holds information for a window */
typedef struct lpxcb_window_t {
    /* The connection associated with this window */
    xcb_connection_t *conn;
    /* The id of this window */
    xcb_drawable_t window;
    /* The id of this parent's window, if it has one. */
    xcb_drawable_t parent;
    /* Damage associated with this window */
    xcb_damage_damage_t damage;
    /* Region - I think we'll use this to add damage to window */
    xcb_xfixes_region_t region;

    /* Should we have something for the pixmap? */
} lpxcb_window_t;

/* Display */
typedef struct lpxcb_connection_t {
    /* The XCB connection */
    xcb_connection_t *conn;
    /* Damaged lpxcb_window_t */
    lpxcb_window_t *damaged;
} lpxcb_connection_t;

/**
 * Return the lpxcb_connection_t that contains the given
 * xcb_connection_t
 * @param conn The connection
 * @return The matching lpxcb_connection_t
 */
lpxcb_connection_t *
lpxcb_find_connection (xcb_connection_t *conn);

/**
 * Add a connection to the connection table.
 * @param conn The connection to add
 * @return The lpxcb_connection containing conn
 */
lpxcb_connection_t *
lpxcb_add_connection (xcb_connection_t *conn);

#endif  /* _LPXCB_API_H_ */
