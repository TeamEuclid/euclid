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

/* Find a window in the table */
lpxcb_window_t *
lpxcb_find_window (xcb_connection_t *conn, xcb_window_t window);

/* Add a window to the table */
lpxcb_window_t *
lpxcb_add_window (xcb_connection_t *conn, xcb_window_t window);

/* Remove a window from the table */
void
lpxcb_remove_window (xcb_connection_t *conn, xcb_window_t window);


#endif  /* _LPXCB_TABLE_H_ */
