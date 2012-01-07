/* Copyright (c) 2011 Jess VanDerwalker
 *
 *  lpxcb_table.c
 */

#include <stdlib.h>
#include "lpxcb_util.h"
#include "lpxcb_table.h"
#include "lpxcb_damage.h"

/* We'll use a simple double linked list for now as our data structure
 * to hold the windows were "managing" */

lpxcb_window_t *
lpxcb_add_window (xcb_connection_t *conn, xcb_window_t window)
{
    lpxcb_window_t *lpxcb_window = NULL;
    xcb_get_geometry_reply_t *geom;
    table_node_t *new;
    table_node_t *curr;
    table_node_t *prev;
    xcb_void_cookie_t cookie;
    xcb_damage_damage_t damage;
    xcb_get_window_attributes_reply_t *attrs;
    xcb_rectangle_t rect;
    
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
    geom = lpxcb_get_window_geometry(conn, window);
    lpxcb_set_window_dimensions(conn, window,
                                geom->x, geom->y, geom->height,
                                geom->width, geom->border_width);

    /* Set the damage rectangle */
    lpxcb_window->damage_rect.x = 0;
    lpxcb_window->damage_rect.y = 0;
    lpxcb_window->damage_rect.width = 0;
    lpxcb_window->damage_rect.height = 0;
    
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

    /* Set up our damage */
    damage = xcb_generate_id(conn);
    cookie = xcb_damage_create_checked(conn, damage, window,
                                           XCB_DAMAGE_REPORT_LEVEL_BOUNDING_BOX);
    if (lpxcb_check_request(conn, cookie, "Could not create new Damage")) {
        return NULL;
    }
    lpxcb_window->damage = damage;
    
    /* Create region that I think we'll use to track the damage area */
    lpxcb_window->region = xcb_generate_id(conn);
    rect.x = 0;
    rect.y = 0;
    rect.width = 0;
    rect.height= 0;

    cookie = xcb_xfixes_create_region_checked(conn, lpxcb_window->region, 1, &rect);
    if (lpxcb_check_request(conn, cookie, "Could not set region")) {
            return NULL;
    }

    lpxcb_window->repair = xcb_generate_id(conn);
    cookie = xcb_xfixes_create_region_checked(conn, lpxcb_window->repair, 1, &rect);
    if (lpxcb_check_request(conn, cookie, "Could not set region")) {
            return NULL;
    }

    /* If the window is viewable, then add damage to it */
    attrs = lpxcb_get_window_attrs(conn, lpxcb_window->window);
    if (attrs->map_state == XCB_MAP_STATE_VIEWABLE) {
        /* TODO: Need to figure out why we are using the dimentions
         * that we are */
        lpxcb_damage_window(lpxcb_window, 0, 0, geom->width, geom->height);
    }
    free(geom);
    free(attrs);
    
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

/* Connection table functions */

lpxcb_connection_t *
lpxcb_find_connection (xcb_connection_t *conn)
{
    conn_node_t *curr;

    curr = conn_table;
    while (curr) {
        if (curr->lpxcb_conn->conn == conn) {
            return curr->lpxcb_conn;
        }
        curr = curr->next;
    }

    return lpxcb_add_connection(conn);
}

lpxcb_connection_t *
lpxcb_add_connection (xcb_connection_t *conn)
{
    lpxcb_connection_t *lpxcb_conn = NULL;
    conn_node_t *new;
    conn_node_t *curr;
    conn_node_t *prev = NULL;
    xcb_damage_query_version_cookie_t dmg_ver_cookie;
    xcb_damage_query_version_reply_t *dmg_ver_reply;
    xcb_xfixes_query_version_cookie_t xfix_ver_cookie;
    xcb_xfixes_query_version_reply_t *xfix_ver_reply;

    new = malloc(sizeof(conn_node_t));
    if (!new) {
        return NULL;
    }
    new->next = NULL;
    new->prev = NULL;

    lpxcb_conn = malloc(sizeof(lpxcb_connection_t));
    if (!lpxcb_conn) {
        return NULL;
    }
    lpxcb_conn->conn = conn;
    lpxcb_conn->damaged = NULL;
    new->lpxcb_conn = lpxcb_conn;

    /* Do the initializations for xfixes and damage extensions */
    dmg_ver_cookie = xcb_damage_query_version(conn, 1, 1);
    dmg_ver_reply = xcb_damage_query_version_reply(conn, dmg_ver_cookie, NULL);

    xfix_ver_cookie = xcb_xfixes_query_version(conn, 4, 0);
    xfix_ver_reply = xcb_xfixes_query_version_reply(conn, xfix_ver_cookie, NULL);
    free(xfix_ver_reply);


    if (!conn_table)  {
        conn_table = new;
        return lpxcb_conn;
    }

    curr = conn_table;
    while (curr) {
        prev = curr;
        curr = curr->next;
    }
    prev->next = new;
    new->prev = prev;
    
    return lpxcb_conn;
}
