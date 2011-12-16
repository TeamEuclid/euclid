/* Copyright (c) 2011 Jess VanDerwalker
 *
 *  lpxcb_table.c
 */

#include <stdlib.h>
#include "lpxcb_table.h"

/* We'll use a simple double linked list for now as our data structure
 * to hold the windows were "managing" */

lpxcb_window_t *
lpxcb_add_window (xcb_connection_t *conn, xcb_window_t window)
{
    lpxcb_window_t *lpxcb_window = NULL;
    table_node_t *new;
    table_node_t *curr;
    table_node_t *prev;
    xcb_void_cookie_t dmg_cookie;
    xcb_damage_damage_t damage;
    xcb_generic_error_t *error;

    /* Does the window already exist */
    lpxcb_window = lpxcb_find_window(conn, window);
    if (lpxcb_window) {
        return lpxcb_window;
    }

    /* Create new lpxcb_window to hold window */
    lpxcb_window = malloc(sizeof(lpxcb_window_t));
    if (!lpxcb_window) {
        exit(1);
    }
    lpxcb_window->conn = conn;
    lpxcb_window->window = window;
    lpxcb_window->parent = 0; /* Not used at the moment */
    lpxcb_window->damage = 0;

    /* Create node to hold the new lpxcb_window */
    new = malloc(sizeof(table_node_t));
    if (!new) {
        exit(1);                /* Should we handle this differently? */
    }
    new->entry = lpxcb_window;

    /* Handle the case where this is the first node added */
    if (!window_table) {
        new->prev = NULL;
        new->next = NULL;
        window_table = new;
    } else {
        curr = window_table;
        while (curr->next) {
            prev = curr;
            curr = curr->next;
        }
        curr->next = new;
        new->prev = curr;
        new->next = NULL;
    }

    /* Check the damage version */
    /* xcb_damage_query_version_cookie_t dmg_ver_cookie; */
    /* xcb_damage_query_version_reply_t *dmg_ver_reply; */
    /* dmg_ver_cookie = xcb_damage_query_version(conn, 1, 1); */
    /* dmg_ver_reply = xcb_damage_query_version_reply(conn, dmg_ver_cookie, NULL); */

    /* Set up our damage */
    damage = xcb_generate_id(conn);
    dmg_cookie = xcb_damage_create_checked(conn, damage, window,
                                           XCB_DAMAGE_REPORT_LEVEL_BOUNDING_BOX);
    error = xcb_request_check(conn, dmg_cookie);
    if (error) {
        fprintf(stderr, "ERROR: Could not create new Damage: %d\n",
                error->error_code);
        return NULL;
    }
    lpxcb_window->damage = damage;
    
    /* Create region that I think we'll use to track damage */
    lpxcb_window->region = xcb_generate_id(conn);

    return lpxcb_window;
}       


lpxcb_window_t *
lpxcb_find_window (xcb_connection_t *conn,  xcb_window_t window)
{
    table_node_t *curr;

    if (!window_table) {
        return NULL;
    }

    curr = window_table;
    while (curr) {
        if (curr->entry->window == window) {
            return curr->entry;
        }
        curr = curr->next;
    }
    return NULL;
}

void
lpxcb_remove_window (xcb_connection_t *conn, xcb_window_t window)
{
    table_node_t *curr;

    curr = window_table;
    while (curr) {
        if (curr->entry->window == window) {
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
