/* Copyright (c) 2012 Jess VanDerwalker
 *
 * lpxcb_data.h
 */

#ifndef _LPXCB_DATA_H_
#define _LPXCB_DATA_H_

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/damage.h>
#include <xcb/xfixes.h>
#include <xcb/xcb_aux.h>

/* Holds information for a window */
typedef struct lpxcb_window_t {
    /* The connection associated with this window */
    xcb_connection_t *conn;
    /* The id of this window */
    xcb_drawable_t window;
    /* The id of this parent's window, if it has one. */
    xcb_drawable_t parent;
    /* Rectangle used to manage damage */
    xcb_rectangle_t damage_rect;
    /* Damage associated with this window */
    xcb_damage_damage_t damage;
    /* Region of window marked as damage area */
    xcb_xfixes_region_t region;
    xcb_xfixes_region_t repair;
    /* The next window with damage */
    struct lpxcb_window_t *next_damaged;
    /* Should we have something for the pixmap? */
} lpxcb_window_t;


/* Node to hold window in table */
typedef struct table_node_t {
    lpxcb_window_t *entry;
    struct table_node_t *prev;
    struct table_node_t *next;
} table_node_t;


/* Connection */
typedef struct lpxcb_connection_t {
    /* The XCB connection */
    xcb_connection_t *conn;
    /* Damaged lpxcb_window_t */
    lpxcb_window_t *damaged;
} lpxcb_connection_t;


/* Node for connection data structure */
typedef struct conn_node_t {
    lpxcb_connection_t *lpxcb_conn;
    struct conn_node_t *next;
    struct conn_node_t *prev;
} conn_node_t;


#endif  /* _LPXCB_DATA_H_ */
