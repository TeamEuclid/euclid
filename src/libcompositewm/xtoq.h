/* Copyright (c) 2012 David Snyder
 *
 * xtoq.h
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


#ifndef _XTOQ_H_
#define _XTOQ_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include <xcb/damage.h>
#include <xcb/composite.h>
#include <xcb/xtest.h>
#include <xcb/xfixes.h>
#include "data.h"
#include <xcb/xcb_keysyms.h>
#include "xtoq_internal.h"

/**
 * Context which contains the display's root window.
 */
extern xtoq_context_t *root_context;

/**
 * Sets up the connection and grabs the root window from the specified screen
 * @param display the display to connect to
 * @return The root context which contains the root window
 */
xtoq_context_t *
xtoq_init(char *display);

/**
 * Returns a window's entire image
 * @param an xtoq_context_t 
 * FIXME: this might be for the root window
 * @return an xtoq_image_t with an the image of a window
 */
xtoq_image_t *
xtoq_get_image(xtoq_context_t *context);

/**
 * Intended for servicing to a client's reaction to a damage notification
 * this window returns the modified subrectangle of a window
 * @param an xtoq_context_t of the damaged window
 * @return an xtoq_image_t with partial image window contents
 */
xtoq_image_t *
test_xtoq_get_image(xtoq_context_t * context);


/**
 * free the memory used by an xtoq_image_t created 
 * during a call to test_xtoq_image_create
 * @param xtoq_image an image to be freed
 */
void 
xtoq_image_destroy(xtoq_image_t * xtoq_image);

/**
 * Set input focus to the window in context
 * @param context The context containing the window
 */
void
xtoq_set_input_focus(xtoq_context_t *context);

/**
 * Set a window to the bottom of the window stack.
 * @param context The context containing the window
 */
void
xtoq_set_window_to_bottom(xtoq_context_t *context);

/**
 * Set a window to the top of the window stack.
 * @param context The context containing the window
 */
void
xtoq_set_window_to_top(xtoq_context_t *context);

/**
 * Starts the event loop and listens on the connection specified in
 * the given xtoq_context_t. Uses callback as the function to call
 * when an event of interest is received. Callback must be able to
 * take an xtoq_event_t as its one and only parameter.
 * @param context The context containing the connection to listen
 * for events on.
 * @param callback The function to call when an event of interest is
 * received.
 * @return Uses the return value of the call to pthread_create as 
 * the return value.
 */
int
xtoq_start_event_loop (xtoq_context_t *context, xtoq_event_cb_t callback);

/**
 * Request a lock on the mutex for the event loop thread. Blocks
 * until lock is aquired, or error occurs.
 * @return 0 if successful, otherwise non-zero
 */
int
xtoq_get_event_thread_lock (void);

/**
 * Release the lock on the mutex for the event loop thread.
 * @return 0 if successsful, otherwise non-zero
 */
int
xtoq_release_event_thread_lock(void);

/**
 * Remove the damage from the given context.
 * @param context The context to remove the damage from
 */
void
xtoq_remove_context_damage(xtoq_context_t *context);

/**
 * Closes the windows open on the X Server, the connection, and the event
 * loop. 
 */
void 
xtoq_close(void);

/**
 * function
 * @param context xtoq_context_t 
 * @param window The window that the key press was made in.
 * @param keyCode The key pressed.
 */
void
xtoq_key_press (xtoq_context_t *context, int window, uint8_t code);

/**
 * function
 * @param context xtoq_context_t 
 * @param window The window that the key press was made in.
 * @param keyCode The key released.
 */
void
xtoq_key_release (xtoq_context_t *context, int window, uint8_t code);

/**
 * Uses the XTEST protocol to send input events to the X Server (The X Server
 * is usually in the position of sending input events to a client). The client
 * will often choose to send coordinates through mouse motion and set the params 
 * x & y to 0 here.
 * @param context xtoq_context_t 
 * @param x - x coordinate
 * @param y - y coordinate
 * @param window The window that the key press was made in.
 */
void
xtoq_button_press (xtoq_context_t *context, long x, long y, int window, int button);

/**
 * Uses the XTEST protocol to send input events to the X Server (The X Server
 * is usually in the position of sending input events to a client). The client
 * will often choose to send coordinates through mouse motion and set the params 
 * x & y to 0 here.
 * @param context xtoq_context_t 
 * @param x - x coordinate
 * @param y - y coordinate
 * @param window The window that the key release was made in.
 */
void
xtoq_button_release (xtoq_context_t *context, long x, long y, int window, int button);

/**
 * function
 * @param context xtoq_context_t 
 * @param x - x coordinate
 * @param y - y coordinate
 * @param window The window that the key release was made in.
 */
void
xtoq_mouse_motion (xtoq_context_t *context, long x, long y, int window, int button);

/****************
 * window.c
 ****************/

/**
 * kill the window, if possible using WM_DELETE_WINDOW (icccm) 
 * otherwise using xcb_kill_client.
 * @param context The context of the window to be killed
 */
void
xtoq_request_close(xtoq_context_t *context);

/**
 * move and/or resize the window, update the context 
 * @param context the context of the window to configure
 * @param x The new x coordinate
 * @param y The new y coordinate
 * @param height The new height
 * @param width The new width
 */
void
xtoq_configure_window(xtoq_context_t *context, int x, int y, int height, int width);

#endif // _XTOQ_H_
