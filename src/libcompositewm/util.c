/* Copyright (c) 2012 Jess VanDerwalker
 *
 * util.c
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "xtoq_internal.h"
#include <xcb/xcb.h>


xcb_get_window_attributes_reply_t *
_xtoq_get_window_attributes (xcb_connection_t *conn, xcb_window_t window)
{
    xcb_get_window_attributes_reply_t *reply;
    xcb_generic_error_t *error;
    xcb_get_window_attributes_cookie_t cookie;

    cookie = xcb_get_window_attributes(conn, window);
    reply = xcb_get_window_attributes_reply(conn, cookie, &error);
    if (error) {
        fprintf(stderr, "ERROR: Failed to get window attributes: %d\n",
                error->error_code);
        return NULL;
    }
    return reply;
}

xcb_get_geometry_reply_t *
_xtoq_get_window_geometry (xcb_connection_t *conn, xcb_window_t window)
{
    xcb_get_geometry_cookie_t cookie;
    cookie = xcb_get_geometry(conn, window);
    return xcb_get_geometry_reply(conn, cookie, NULL);
}


void
_xtoq_write_all_children_window_info (xcb_connection_t *conn,
									  xcb_window_t root)
{

    xcb_query_tree_reply_t *reply;
    xcb_query_tree_cookie_t tree_cookie;
    xcb_window_t *children;     /* The children of the given root */
	image_data_t img_data;
	xcb_generic_error_t *error;
    int len;
    int i;

    tree_cookie = xcb_query_tree(conn, root);
    reply = xcb_query_tree_reply(conn, tree_cookie, &error);
    if (error) {
        fprintf(stderr, "ERROR: Failed to get query tree: %d\n",
                error->error_code);
        return;
    }
    /* Get the number of children */
    len = xcb_query_tree_children_length(reply);
    children = xcb_query_tree_children(reply);

    /* Iterate thorough all the children and get their pixmap (hopefully) */
	printf("--- Iterating through children of window %u ---\n",
		   root);
    for (i = 0; i < len; i++) {
        _xtoq_write_window_info(conn, children[i]);
		img_data = _xtoq_get_window_image_data(conn, children[i]);
		if (!img_data.data) {
			printf("Image data is empty\n");
		}
    }
	printf("--- End window iteration ---\n");
    
    /* Free the stuff allocated by XCB */
    free(reply);
}

image_data_t
_xtoq_get_window_image_data (xcb_connection_t *conn, xcb_drawable_t window)
{
    image_data_t image_data;
    xcb_get_image_cookie_t img_cookie;
    xcb_get_image_reply_t *reply;
    xcb_generic_error_t *error;
    xcb_get_geometry_reply_t *geom_reply;

    image_data.data = NULL;
    image_data.length = 0;

    geom_reply = _xtoq_get_window_geometry(conn, window);
    if (!geom_reply) {
        fprintf(stderr, "ERROR: Failed to get window image data.\n");
        return image_data;
    }

    img_cookie = xcb_get_image(conn,
                               XCB_IMAGE_FORMAT_Z_PIXMAP,
                               window,
                               0,
                               0,
                               geom_reply->width,
                               geom_reply->height,
                               (unsigned int) ~0L);

    reply = xcb_get_image_reply(conn, img_cookie, &error);
    if (error) {
        fprintf(stderr, "ERROR: Failed to get window image data reply: %d\n",
                error->error_code);
        return image_data;
    }
    image_data.data = xcb_get_image_data(reply);
    image_data.length = xcb_get_image_data_length(reply);

    free(geom_reply);
    /* free(reply); */

    return image_data;
}

void
_xtoq_write_window_info (xcb_connection_t *conn, xcb_window_t window)
{
    xcb_get_geometry_reply_t *geom_reply;
    xcb_get_window_attributes_reply_t *attr_reply;

    geom_reply = _xtoq_get_window_geometry(conn, window);
    if (!geom_reply) {
        printf("Failed to get geometry for window %u\n", window);
        return;
    }
    attr_reply = _xtoq_get_window_attributes(conn, window);
    if (!attr_reply) {
        printf("Failed to get attributes for window %u\n", window);
        return;
    }

    /* Print out the geometry and attributes we're interested in */
    printf("Window Id: %u\n", window);
    printf("x: %d\ty: %d\n", geom_reply->x, geom_reply->y);
    printf("width: %d\theight: %d\n", geom_reply->width, geom_reply->height);

    printf("Map state: ");
    if (attr_reply->map_state ==  XCB_MAP_STATE_UNMAPPED) {
        printf("Unmapped\n");
    } else if (attr_reply->map_state == XCB_MAP_STATE_UNVIEWABLE) {
        printf("Unviewable\n");
    } else {
        printf("Viewable\n");
    }

    free(geom_reply);
    free(attr_reply);
}

int
_xtoq_request_check (xcb_connection_t *conn, xcb_void_cookie_t cookie,
                     char *msg)
{
    xcb_generic_error_t *error;

    error = xcb_request_check(conn, cookie);
    if (error) {
        if (msg) {
            fprintf(stderr, "ERROR: ");
            fprintf(stderr,"%s", msg);
            fprintf(stderr, "\nError code: %d\n", error->error_code);
        }
        return error->error_code;
    }
    return 0;
}
