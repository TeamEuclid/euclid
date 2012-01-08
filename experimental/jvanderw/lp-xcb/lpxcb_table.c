/* Copyright (c) 2011 Jess VanDerwalker
 *
 *  lpxcb_table.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <xcb/composite.h>
#include "lpxcb_util.h"
#include "lpxcb_table.h"
#include "lpxcb_damage.h"


void
lpxcb_set_root_window (xcb_window_t window)
{
    root_window = window;
}

lpxcb_window_t *
lpxcb_init_window (xcb_connection_t *conn, xcb_window_t win)
{
    lpxcb_window_t *lpxcb_win;
    xcb_void_cookie_t cookie;
    xcb_get_geometry_reply_t *geom;
    xcb_damage_damage_t damage;
    xcb_get_window_attributes_reply_t *attrs;
    xcb_rectangle_t rect;

    lpxcb_win = malloc(sizeof(lpxcb_window_t));
    if (!lpxcb_win) {
        return NULL;
    }
   
    lpxcb_win->conn = conn;
    lpxcb_win->window = win;
    lpxcb_win->parent = 0; /* Not used at the moment */
    lpxcb_win->damage = 0;
    geom = lpxcb_get_window_geometry(conn, win);
    lpxcb_set_window_dimensions(conn, win,
                                geom->x, geom->y,
                                geom->height, geom->width,
                                geom->border_width);

    /* Set the damage rectangle */
    lpxcb_win->damage_rect.x = 0;
    lpxcb_win->damage_rect.y = 0;
    lpxcb_win->damage_rect.width = 0;
    lpxcb_win->damage_rect.height = 0;

    /* Set up our damage */
    damage = xcb_generate_id(conn);
    cookie = xcb_damage_create_checked(conn, damage, win,
                                           XCB_DAMAGE_REPORT_LEVEL_BOUNDING_BOX);
    if (lpxcb_check_request(conn, cookie, "Could not create new Damage")) {
        return NULL;
    }
    lpxcb_win->damage = damage;
    
    /* Create region that I think we'll use to track the damage area */
    lpxcb_win->region = xcb_generate_id(conn);
    rect.x = 0;
    rect.y = 0;
    rect.width = 0;
    rect.height= 0;

    cookie = xcb_xfixes_create_region_checked(conn,
                                              lpxcb_win->region, 
                                              1, &rect);
    if (lpxcb_check_request(conn, cookie, "Could not set region")) {
            return NULL;
    }

    lpxcb_win->repair = xcb_generate_id(conn);
    cookie = xcb_xfixes_create_region_checked(conn,
                                              lpxcb_win->repair,
                                              1, &rect);
    if (lpxcb_check_request(conn, cookie, "Could not set region")) {
            return NULL;
    }

    /* If the window is viewable, then add damage to it */
    attrs = lpxcb_get_window_attrs(conn, lpxcb_win->window);
    if (attrs->map_state == XCB_MAP_STATE_VIEWABLE) {
        /* TODO: Need to figure out why we are using the dimentions
         * that we are */
        lpxcb_damage_window(lpxcb_win, 0, 0, geom->width, geom->height);
    }

    if (lpxcb_win->window != root_window) {
        uint32_t values[] = { 1 };
        xcb_change_window_attributes (conn, lpxcb_win->window,
                                      XCB_CW_OVERRIDE_REDIRECT,
                                      values);
        cookie = xcb_composite_redirect_window_checked(conn,
                                                       lpxcb_win->window,
                                                       1);
        if (lpxcb_check_request(conn, cookie,
                                "Failed to set up compositing for window")) {
        }
    }

    /* Free memory */
    free(geom);
    free(attrs);

    return lpxcb_win;
}


/* We'll use a simple double linked list for now as our data structure
 * to hold the windows were "managing" */

lpxcb_window_t *
lpxcb_add_window (xcb_connection_t *conn, xcb_window_t window)
{
    lpxcb_connection_t *lpxcb_conn;
    lpxcb_window_t *lpxcb_window = NULL;
    table_node_t *new;
    table_node_t *curr;
    table_node_t *prev;

    lpxcb_conn = lpxcb_find_connection(conn);

    /* Does the window already exist */
    lpxcb_window = lpxcb_find_window(conn, window);
    if (lpxcb_window) {
        return lpxcb_window;
    }

    /* Create new lpxcb_window to hold window */
    lpxcb_window =  lpxcb_init_window(conn, window);
    if (!lpxcb_window) {
        exit(1);
    }
    
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
    xcb_composite_query_version_cookie_t comp_ver_cookie;
    xcb_composite_query_version_reply_t *comp_ver_reply;

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

    comp_ver_cookie = xcb_composite_query_version(conn,
                                                  XCB_COMPOSITE_MAJOR_VERSION,
                                                  XCB_COMPOSITE_MINOR_VERSION);
    comp_ver_reply = xcb_composite_query_version_reply(conn, comp_ver_cookie, NULL);
    if (!comp_ver_reply) {
        fprintf(stderr, "Composite extenstion is not available");
    } else {
        free(comp_ver_reply);
    }

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
