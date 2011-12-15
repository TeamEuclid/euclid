/* Copyright (c) 2011 Jess VanDerwalker
 *
 *  lpxcb_table.c
 */

#include <stdlib.h>
#include "lpxcb_table.h"

/* We'll use a simple double linked list for now as our data structure
 * to hold the windows were "managing" */

void
lpxcb_add_window (xcb_connection_t *conn, lpxcb_window_t *window)
{
    table_node_t *new;
    table_node_t *curr;
    table_node_t *prev;

    if (lpxcb_find_window(conn, window) != 0) {
        return;
    }

    new = malloc(sizeof(table_node_t));
    if (new == NULL) {
        exit(1);
    }

    if (window_table == NULL) {
        /* Handle the case where this is the first node added */
        new->prev = NULL;
        new->entry = window;
        new->next = NULL;
    } else {
        curr = window_table;
        while (curr->next != NULL) {
            prev = curr;
            curr = curr->next;
        }
        curr->next = new;
        new->prev = curr;
    }
    return;
}       


lpxcb_window_t
lpxcb_find_window (xcb_connection_t *conn, lpxcb_window_t *window)
{
    table_node_t *curr;

    if (window_table = NULL) {
        return NULL;
    }

    curr = window_table;
    while (curr) {
        if (curr->entry == window) {
            return window;
        }
        curr = curr->next;
    }
    return NULL;
}

void
lpxcb_remove_window (xcb_connection_t *conn, lpxcb_window_t *window)
{
    table_node_t *curr;
    table_node_t *prev;

    curr = window_table;
    while (curr) {
        if (curr->entry == window) {
            curr->next->prev = curr->prev;
            if (curr->prev) {
                curr->prev->next = curr->next;
            }
            free(curr);
            return;
        }
    }
    return;
}
