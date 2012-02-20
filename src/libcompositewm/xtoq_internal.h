/* Copyright (c) 2012 Jess VanDerwalker <washu@sonic.net>
 * All rights reserved.
 *
 * xtoq_internal.h
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

#ifndef _XTOQ_INTERNAL_H_
#define _XTOQ_INTERNAL_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include "data.h"
#include "xtoq.h"


/**
 * Strucuture used to pass nesessary data to xtoq_start_event_loop.
 */
typedef struct xtoq_event_connetion {
	xcb_connection_t *conn;	          /* Connection to listen to events on */
	xtoq_event_cb_t event_callback;   /* Fuction to call when event caught */
} xtoq_event_connection;

/* util.c */

/**
 * Return the given windows attributes reply. Caller must free memory
 * allocated for reply.
 * @param conn The windows connection.
 * @param window The window.
 * @return The window attributes reply. Null if the request fails.
 */
xcb_get_window_attributes_reply_t *
_xtoq_get_window_attributes (xcb_connection_t *conn, xcb_window_t window);

/**
 * Return the geometry of the window in a geometry reply. Caller must free
 * memory allocated for reply.
 * @param conn The windows connection.
 * @param window The window.
 * @return The window's geometry reply. Null if the request for reply fails.
 */
xcb_get_geometry_reply_t *
_xtoq_get_window_geometry (xcb_connection_t *conn, xcb_window_t window);

/**
 * Print out information about the existing windows attached to our
 * root. Most of this code is taken from src/manage.c from the i3 code
 * by Michael Stapelberg
 */
void
_xtoq_write_all_children_window_info (xcb_connection_t *conn,
								 xcb_window_t root);

/**
 * Get the image data for a window.
 * @param conn The connection to the xserver.
 * @param window The window.
 * @return Structure containing data and data length
 */
image_data_t
_xtoq_get_window_image_data (xcb_connection_t *conn, xcb_window_t window);

/**
 * Write information about a window out to stdio.
 * TODO: Add the ability to pass in the stream to write to.
 * @param conn The connection with the window.
 * @param window The window.
 */
void
_xtoq_write_window_info (xcb_connection_t *conn, xcb_window_t window);

/**
 * Check the request cookie and determine if there is an error.
 * @param conn The connection the request was sent on.
 * @param cookie The cookie returned by the request.
 * @param msg the string to display if there is an error with the request.
 * @return int The number of the error code, if any. Otherwise zero.
 */
int
_xtoq_request_check (xcb_connection_t *conn, xcb_void_cookie_t cookie,
              char *msg);

/****************
 * init.c
 ****************/

xcb_query_extension_reply_t * 
_xtoq_init_extension(xcb_connection_t *conn, char *extension_name);

void 
_xtoq_init_damage(xtoq_context_t *contxt);

void
_xtoq_init_xfixes (xtoq_context_t *contxt);

/****************
 * event_loop.c
 ****************/

/**
 * Starts the event loop thread which listens on the given connection and
 * calls event_callback if an event is caught.
 * @param *conn The connection to listen for X events on
 * @param event_callback The callback function to call when a event is caught.
 */
int
_xtoq_start_event_loop (xcb_connection_t *conn,
						xtoq_event_cb_t event_callback);

/****************
 * context_list.c
 ****************/

typedef struct _xtoq_context_node {
    struct xtoq_context_t *context;
    struct _xtoq_context_node * next;
    struct _xtoq_context_node * prev;
} _xtoq_context_node;

typedef struct _xtoq_context_list {
    struct _xtoq_context_node * head;
    int count;
} _xtoq_context_list;

/* this is the head pointer */
extern _xtoq_context_node *_xtoq_window_list_head;


xtoq_context_t * 
_xtoq_add_context_t(struct xtoq_context_t *context);

void
_xtoq_remove_context_node(xcb_window_t window_id);

_xtoq_context_node *
_xtoq_get_context_node_by_window_id (xcb_window_t window_id);

/****************
 * window.c
 ****************/
xtoq_context_t *_xtoq_window_created(xcb_connection_t * conn, xcb_create_notify_event_t *evt);


#endif  /* _UTIL_H_ */
