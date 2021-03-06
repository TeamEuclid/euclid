/* Copyright (c) 2012 David Snyder, Benjamin Carr, Braden Wooley
 *
 * rootimg_api.h
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

/* #ifndef _XTOQ_C_ */
/* #define _XTOQ_C_ */

#include "xtoq.h"
#include "xtoq_internal.h"
#include "X11/keysym.h"
#include <string.h>

// aaron key stuff
#define XK_Shift_L                       0xffe1
xcb_key_symbols_t *syms = NULL;
// end aaron key stuff
xtoq_context_t *root_context = NULL;

// This init function needs set the window to be registered for events!
// First one we should handle is damage
xtoq_context_t *
xtoq_init(char *display) {
    
    xcb_connection_t *conn;
    int conn_screen;
    xcb_screen_t *root_screen;
    xcb_drawable_t root_window;
	xcb_void_cookie_t cookie;
    uint32_t mask_values[1];
 
    conn = xcb_connect(display, &conn_screen);
    
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;
    
    // Set the mask for the root window so we know when new windows
    // are created on the root. This is where we add masks for the events
    // we care about catching on the root window.
    mask_values[0] = XCB_EVENT_MASK_KEY_PRESS |
                     XCB_EVENT_MASK_KEY_RELEASE |
                     XCB_EVENT_MASK_BUTTON_PRESS |
                     XCB_EVENT_MASK_BUTTON_RELEASE |
                     XCB_EVENT_MASK_POINTER_MOTION |
		             XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                     XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                     XCB_EVENT_MASK_ENTER_WINDOW | 
                     XCB_EVENT_MASK_LEAVE_WINDOW | 
		             XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT;
    cookie = xcb_change_window_attributes_checked(conn, root_window,
												  XCB_CW_EVENT_MASK,
												  mask_values);
	if (_xtoq_request_check(conn, cookie, "Could not set root window mask.")) {
			fprintf(stderr, "Is another window manager running?\n");
			xcb_disconnect(conn);
			exit(1);
	}

	xcb_flush(conn);

    root_context = malloc(sizeof(xtoq_context_t));
    root_context->conn = conn;
    root_context->parent = 0;
    root_context->window = root_window;
    
    // Set width, height, x, & y from root_screen into the xtoq_context_t
    root_context->width = root_screen->width_in_pixels;
    root_context->height = root_screen->height_in_pixels;
    root_context->x = 0;
    root_context->y = 0;

    _xtoq_init_composite(root_context);
    
    _xtoq_init_damage(root_context);
    
    _xtoq_init_xfixes(root_context);
    
    _xtoq_add_context_t(root_context);
        
    syms = xcb_key_symbols_alloc(conn);
    _xtoq_init_extension(conn, "XTEST");
	_xtoq_init_extension(conn, "XKEYBOARD");

	_xtoq_get_wm_atoms(root_context);

    return root_context;
}

xtoq_image_t *
xtoq_get_image(xtoq_context_t *context) {
    
    xcb_get_geometry_reply_t *geom_reply;
    
    xcb_image_t *image;
    
    geom_reply = _xtoq_get_window_geometry(context->conn, context->window);
    
    //FIXME - right size
    xtoq_image_t * xtoq_image = (xtoq_image_t *) malloc(10 * sizeof (xtoq_image_t));
    
	xcb_flush(context->conn);
    /* Get the image of the root window */
    image = xcb_image_get(context->conn,
                          context->window,
                          geom_reply->x,
                          geom_reply->y,
                          geom_reply->width,
                          geom_reply->height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    
    xtoq_image->image = image;
    xtoq_image->x = geom_reply->x;
    xtoq_image->y = geom_reply->y;
    xtoq_image->width = geom_reply->width;
    xtoq_image->height = geom_reply->height;
    
    free(geom_reply);
    
    return xtoq_image;
}

int 
xtoq_start_event_loop (xtoq_context_t *context,
                       xtoq_event_cb_t callback)
{
	/* Simply call our internal function to do the actual setup */
	return _xtoq_start_event_loop(context->conn, callback);
}

xtoq_image_t *
test_xtoq_get_image(xtoq_context_t *context) {
    
    xcb_image_t *image;
    
	xcb_flush(context->conn);
    /* Get the image of the root window */
    image = xcb_image_get(context->conn,
                          context->window,
                          context->damaged_x,
                          context->damaged_y,
                          context->damaged_width,
                          context->damaged_height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    
    //FIXME - Calculate memory size correctly
    xtoq_image_t * xtoq_image = (xtoq_image_t *) malloc(10 * sizeof (xtoq_image_t));
    
    xtoq_image->image = image;
    xtoq_image->x = context->damaged_x;
    xtoq_image->y = context->damaged_y;
    xtoq_image->width = context->damaged_width;
    xtoq_image->height = context->damaged_height;

    return xtoq_image;
}


void 
xtoq_image_destroy(xtoq_image_t * xtoq_image){

    xcb_image_destroy(xtoq_image->image);
    free(xtoq_image);
}
 

void
xtoq_remove_context_damage(xtoq_context_t *context)
{
	xcb_xfixes_region_t region = xcb_generate_id(context->conn);
	xcb_rectangle_t rect;
	xcb_void_cookie_t cookie;

	if (!context) {
		return;
	}

	rect.x = context->damaged_x;
	rect.y = context->damaged_y;
	rect.width = context->damaged_width;
	rect.height = context->damaged_height;

	xcb_xfixes_create_region(root_context->conn,
							 region,
							 1, 
							 &rect);
            
	cookie = xcb_damage_subtract_checked (context->conn,
										  context->damage,
										  region,
										  0);

	if (!(_xtoq_request_check(context->conn, cookie,
							  "Failed to subtract damage"))) {
		context->damaged_x = 0;
		context->damaged_y = 0;
		context->damaged_width = 0;
		context->damaged_height = 0;
	}
	return;
}

/* Close all windows, the connection, as well as the event loop */
void xtoq_close(void) {
    
    _xtoq_context_node *head = _xtoq_window_list_head;
    xcb_connection_t *conn = head->context->conn;
    xcb_flush(conn);
    
    // Close all windows
    while(head) {
        xtoq_request_close(head->context);
        _xtoq_window_list_head = head->next;
        free(head);
        head = _xtoq_window_list_head;
    }
    
    // Disconnect from the display
    xcb_disconnect(conn);
    
    // Terminate the event loop
    int ret = _xtoq_stop_event_loop();
    if (ret != 1) printf("Event loop failed to close\n");
    
    return;
    
}

