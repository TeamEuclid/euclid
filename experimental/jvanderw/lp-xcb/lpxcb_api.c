/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_api.c
 */

#include "lpxcb_api.h"


/* The code for the function is taken from i3 */
void
reparent_existing_windows(xcb_connection_t *conn, xcb_window_t root)
{
    xcb_query_tree_reply_t *tree_reply;
    xcb_window_t *children;
    xcb_get_window_attributes_cookie_t *attr_cookies;
    int i;
    int length;

    reply = xcb_query_tree_reply(conn, xcb_query_tree(conn, root));
    if (!reply) {
        printf("No reply from xcb_query_tree.\n");
        return;
    }

    length = xcb_query_tree_children_length(reply);
    /* Allocate memory for the cookies */
    attr_cookies = calloc(sizeof(*attr_cookies), length);

    /* Get the attributes from every child window of the root */
    children = xcb_query_tree_children(reply);
    for (i = 0; i < length; i++) {
        attr_cookies[i] = xcb_get_window_attributes(conn, children[i]);
    }

    /* Now, reparent each of the individuals */
    for (i = 0; i < length; i++) 
    
    free(reply);
    free(attr_cookies);
}

lpxcb_window *
lpxcb_find_window (xcb_connection_t *conn, xcb_window_t window)
{
    lpxcb_window *lpxcb_window;
    
    return lpxcb_window;
}

lpxcb_window *
lpxcb_add_window (xcb_connection_t *conn, xcb_window_t window)
{
    lpxcb_window *lpxcb_window;

    return lpxcb_window;
}
