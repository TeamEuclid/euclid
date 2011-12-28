/* Copyright (c) 2011 Jess VanDerwalker
 *
 *  lpxcb_table.h
 */

#ifndef _LPXCB_TABLE_H_
#define _LPXCB_TABLE_H_

#include <xcb/damage.h>
#include "lpxcb_api.h"

/* Node to hold window in table */
typedef struct table_node_t {
    lpxcb_window_t *entry;
    struct table_node_t *prev;
    struct table_node_t *next;
} table_node_t;

/* Data structure to hold managed windows */
table_node_t *window_table;

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

/* Data structure to hold the connections */
conn_node_t *conn_table;

/* Find a window in the table */
lpxcb_window_t *
lpxcb_find_window (xcb_connection_t *conn, xcb_window_t window);

/* Add a window to the table */
lpxcb_window_t *
lpxcb_add_window (xcb_connection_t *conn, xcb_window_t window);

/* Remove a window from the table */
void
lpxcb_remove_window (xcb_connection_t *conn, xcb_window_t window);

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


#endif  /* _LPXCB_TABLE_H_ */
