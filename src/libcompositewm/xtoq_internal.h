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
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_atom.h>
#include "data.h"
#include "xtoq.h"


/**
 * Strucuture used to pass nesessary data to xtoq_start_event_loop.
 */
typedef struct xtoq_event_connetion {
	xcb_connection_t *conn;	          /* Connection to listen to events on */
	xtoq_event_cb_t event_callback;   /* Fuction to call when event caught */
} xtoq_event_connection;

/**
 * Structure to hold WM_* atoms that we care about
 */
typedef struct xtoq_wm_atoms {
    xcb_atom_t wm_protocols_atom;
    xcb_atom_t wm_delete_window_atom;
} xtoq_wm_atoms;

/**
 * Global for the atoms needed
 */
extern xtoq_wm_atoms *_wm_atoms;

/**
 * Mutex lock supplied to client to lock event loop thread
 */
extern pthread_mutex_t event_thread_lock;

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
 * @param conn a connection the the xserver.
 * @param window the window.
 * @return the geometry of the window
 */
void
_xtoq_write_all_children_window_info (xcb_connection_t *conn,
								 xcb_window_t root);

/**
 * Get the image data for a window.
 * @param conn the connection to the xserver.
 * @param root the window acenstral to all children to be written.
 * @return a structure containing data and data length.
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

/**
 * Initializes an extension on the xserver.
 * @param conn The connection to the xserver.
 * @param extension_name The string specifying the name of the extension.
 * @return The reply structure
 */
xcb_query_extension_reply_t * 
_xtoq_init_extension(xcb_connection_t *conn, char *extension_name);

/**
 * Initializes damage on a window contained in a context.
 * The context will likely contain the root window.
 * @param contxt A context containing a window 
 */
void 
_xtoq_init_damage(xtoq_context_t *contxt);

/**
 * Initializes the composite extension on the context containg
 * the root window.
 * @param contxt The contxt containing the root window
 */
void 
_xtoq_init_composite(xtoq_context_t *contxt);

/**
 * Initialize the xfixes extension.
 * @param contxt The context
 */
void
_xtoq_init_xfixes (xtoq_context_t *contxt);

/**
 * Get the values for the WM_* atoms that we need.
 * @param contxt The context
 */
void
_xtoq_get_wm_atoms (xtoq_context_t *contxt);

/****************
 * event_loop.c
 ****************/

/**
 * Starts the event loop thread which listens on the given connection and
 * calls event_callback if an event is caught.
 * @param *conn The connection to listen for X events on
 * @param event_callback The callback function to call when a event is caught.
 * @return 0 on success, nonzero on failure.
 */
int
_xtoq_start_event_loop (xcb_connection_t *conn,
						xtoq_event_cb_t event_callback);

/**
 * Stops the thread running the event loop.
 * @return 0 on success, otherwise zero.
 */
int
_xtoq_stop_event_loop (void);

/****************
 * context_list.c
 ****************/

/**
 * A structure (doubly linked list) to hold
 * pointers to the contexts
 */
typedef struct _xtoq_context_node {
    struct xtoq_context_t *context;   /**< Pointer to a context */
    struct _xtoq_context_node * next; /**< Pointer to the next context node */
    struct _xtoq_context_node * prev; /**< Pointer to the previous context node */
} _xtoq_context_node;

/* this is the head pointer */
extern _xtoq_context_node *_xtoq_window_list_head;

/**
 * Add a newly created context to the context_list.
 * @param context The context to be added to the linked list
 * @return Pointer to context added to the list.
 */
xtoq_context_t * 
_xtoq_add_context_t(struct xtoq_context_t *context);

/**
 * Remove a context to the context_list using the window's id.
 * @param window_id The window_id of the context which should
 * be removed from the context_list
 */
void
_xtoq_remove_context_node(xcb_window_t window_id);

/**
 * Find a context in the doubly linked list using its window_id.
 * @param window_id The window_id of the context which should
 * @return Pointer to context (if found), NULL if not found.
 */
xtoq_context_t *
_xtoq_get_context_node_by_window_id (xcb_window_t window_id);

/****************
 * window.c
 ****************/

/**
 * Create a new context for the window specified in the event.
 * @param conn The connection to xserver
 * @param evt The map event for the window
 * @return Pointer to new context. NULL if window already exists.
 */
xtoq_context_t *
_xtoq_window_created(xcb_connection_t * conn,
									 xcb_map_request_event_t *evt);
/**
 * Destroy the damage object associated with the window. 
 * Call the remove function in context_list.c
 * @param conn The connection to xserver
 * @param event The destroy notify event for the window
 * @return Pointer to the context that was removed from the list, NULL if
 * window isn't being managed by context_list
 */
xtoq_context_t *
_xtoq_destroy_window(xcb_destroy_notify_event_t *event);

/**
 * Resize the window to given width and height.
 * @param conn The connection
 * @param window The id of window to resize
 * @param width The new width
 * @param height The new height
 */
void
_xtoq_resize_window (xcb_connection_t *conn, xcb_window_t window,
					 int width, int height);

/**
 * Map the given window.
 * @param context The context of the window to map
 */
void
_xtoq_map_window (xtoq_context_t *context);

#endif  /* _XTOQ_INTERNAL_H_ */
