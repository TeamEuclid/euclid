/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_damage.c
 */


#include "lpxcb_damage.h"
#include "lpxcb_table.h"


lpxcb_window_t *
lpxcb_find_damaged (xcb_connection_t *conn)
{
    lpxcb_connection_t *lpxcb_conn;

    lpxcb_conn = lpxcb_find_connection(conn);
    return lpxcb_conn->damaged;
}

/* Most of this code is taken directly from Keith Packard's
 * _xlightpipe_damage_window function in his Lightpipe */
void
lpxcb_damage_window(lpxcb_window_t *lpxcb_window, int x, int y, int width, int height)
{
    lpxcb_connection_t *lpxcb_conn;
    lpxcb_window_t **prev;
    int old_x;
    int new_x;
    int old_y;
    int new_y;
    xcb_rectangle_t rect;
    xcb_xfixes_region_t new_region;
    xcb_void_cookie_t cookie;

    /* Add this window as the "next damaged" in the chain of "damaged"
     * windows. Chain starts with the lpxcb_conn->damaged */
    lpxcb_conn = lpxcb_find_connection(lpxcb_window->conn);
    if (!lpxcb_conn)
        return;
    for (prev = &lpxcb_conn->damaged; *prev; prev = &(*prev)->next_damaged) {
        if ((*prev) == lpxcb_window)
            return;
    }
    lpxcb_window->next_damaged = *prev;
    *prev = lpxcb_window;

    /* If we have no existing values for damage area, just use given
     * dimentions. */
    if (lpxcb_window->damage_rect.width == 0) {
        lpxcb_window->damage_rect.x = x;
        lpxcb_window->damage_rect.y = y;
        lpxcb_window->damage_rect.width = width;
        lpxcb_window->damage_rect.height = height;
    } else {
        /* If we have existing dimentions, have the new dimentions
         * include the area given by the existing dimentions, and the
         * new ones. */
        old_x = lpxcb_window->damage_rect.x + lpxcb_window->damage_rect.width;
        old_y = lpxcb_window->damage_rect.y + lpxcb_window->damage_rect.height;
        new_x = x + width;
        new_y = y + height;

        if (x < lpxcb_window->damage_rect.x)
            lpxcb_window->damage_rect.x = x;
        if (new_x > old_x)
            old_x = new_x;
        lpxcb_window->damage_rect.width = old_x - lpxcb_window->damage_rect.x;
        if (y < lpxcb_window->damage_rect.y)
            lpxcb_window->damage_rect.y = y;
        if (new_y > old_y)
            old_y = new_y;
        lpxcb_window->damage_rect.height = old_y - lpxcb_window->damage_rect.y;
    }

    /* If there is a damage region, union the new rectangle with the existing region */
    if (lpxcb_window->region)
    {
        rect.x = x;
        rect.y = y;
        rect.width = width;
        rect.height = height;

        new_region = xcb_generate_id(lpxcb_conn->conn);
        xcb_xfixes_region_t dest;
        dest = xcb_generate_id(lpxcb_conn->conn);
        cookie = xcb_xfixes_create_region_checked(lpxcb_conn->conn, new_region, 1, &rect);
        if (lpxcb_check_request(lpxcb_conn->conn, cookie,
                                "Failed to set new region from rectangle")) {
            return;
        }
        xcb_aux_sync(lpxcb_conn->conn);
        lpxcb_get_region_rects(lpxcb_conn->conn, new_region);
        lpxcb_get_region_rects(lpxcb_conn->conn, lpxcb_window->region);
        cookie = xcb_xfixes_union_region_checked(lpxcb_conn->conn,
                                                 new_region,
                                                 lpxcb_window->region,
                                                 lpxcb_window->region);
        if (lpxcb_check_request(lpxcb_conn->conn, cookie,
                                "Failed to union regions")) {
            return;
        }
    }
}


/* Most of this code is taken directly from Keith Packard's
 * _xlightpipe_undamage_window function in his Lightpipe */
void
lpxcb_remove_damage (lpxcb_window_t *lpxcb_window)
{
    lpxcb_connection_t *lpxcb_conn;
    lpxcb_window_t **prev;
    xcb_xfixes_region_t empty_region;
    xcb_void_cookie_t cookie;
 
    /* Remove the window from the links of damaged windows */
    lpxcb_conn = lpxcb_find_connection(lpxcb_window->conn);
    if (!lpxcb_conn)
        return;
    for (prev = &lpxcb_conn->damaged; *prev; prev = &(*prev)->next_damaged) {
        if ((*prev) == lpxcb_window)
        {
            *prev = lpxcb_window->next_damaged;
            break;
        }
    }

    /* Clear the damage rectangle */
    lpxcb_window->damage_rect.x = 0;
    lpxcb_window->damage_rect.y = 0;
    lpxcb_window->damage_rect.width = 0;
    lpxcb_window->damage_rect.height = 0;

    if (lpxcb_window->region) {
        empty_region = xcb_generate_id(lpxcb_conn->conn);
        cookie = xcb_xfixes_create_region_checked(lpxcb_conn->conn, empty_region,
                                                  1, &lpxcb_window->damage_rect);
        if (lpxcb_check_request(lpxcb_conn->conn, cookie,
                                "Failed to create empty region")) {
            return;
        }

        cookie = xcb_xfixes_destroy_region_checked(lpxcb_conn->conn,
                                                   empty_region);
        if (lpxcb_check_request(lpxcb_conn->conn, cookie,
                                "Failed to destroy empty region")) {
            return;
        }
    }
}


void
lpxcb_update_window(lpxcb_window_t *lpxcb_window)
{
    xcb_void_cookie_t cookie;

    /* Set the damage dimentions we have into a region */
    cookie = xcb_xfixes_set_region_checked(lpxcb_window->conn,
                                           lpxcb_window->repair,
                                           1,
                                           &lpxcb_window->damage_rect);
    if (lpxcb_check_request(lpxcb_window->conn, cookie,
                            "Failed to set region with damage_rect")) {
        return;
    }

    cookie = xcb_damage_subtract_checked(lpxcb_window->conn,
                                         lpxcb_window->damage,
                                         lpxcb_window->repair,
                                         NULL);
    if (lpxcb_check_request(lpxcb_window->conn, cookie,
                            "Failed to subtract damage")) {
        return;
    }

    /* TODO: Need to copy the image */
}
