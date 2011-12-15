/* Copyright (c) 2011 Jess VanDerwalker
 *
 *  lpxcb_table.h
 */

#ifndef _LPXCB_TABLE_H_
#define _LPXCB_TABLE_H_

#include "lpxcb_api.h"

/* Node to hold window in table */
typedef struct table_node_t {
    lpxcb_window_t *entry;
    window_table_entry *prev;
    window_table_entry *next;
} window_table_entry;

table_node_t *window_table;

lpxcb_window_t
lpxcb_find_window (xcb_connection_t *conn, lpxcb_window_t *window);

void
lpxcb_add_window (xcb_connection_t *conn, lpxcb_window_t *window);

void
lpxcb_remove_window (xcb_connection_t *conn, lpxcb_window_t *window);


#endif  /* _LPXCB_TABLE_H_ */
