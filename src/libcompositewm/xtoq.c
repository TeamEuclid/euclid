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
#include "X11/keysym.h" //aaron
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
                     XCB_EVENT_MASK_BUTTON_PRESS |
                     XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
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

xcb_image_t *
xtoq_get_image(xtoq_context_t *context) {
    
    xcb_get_geometry_reply_t *geom_reply;
    
    //image_data_t img_data;
    xcb_image_t *image;
    
    geom_reply = _xtoq_get_window_geometry(context->conn, context->window);
    
	//xcb_flush(context.conn);
    /* Get the image of the root window */
    image = xcb_image_get(context->conn,
                          context->window,
                          geom_reply->x,
                          geom_reply->y,
                          geom_reply->width,
                          geom_reply->height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    free(geom_reply);
    return image;
}

void
xtoq_free_image(xcb_image_t *img) {
    free(img);
}

int 
xtoq_start_event_loop (xtoq_context_t *context,
                       xtoq_event_cb_t callback)
{
	/* Simply call our internal function to do the actual setup */
	return _xtoq_start_event_loop(context->conn, callback);
}



void
dummy_xtoq_key_press (xtoq_context_t *context, int window, uint8_t code)
{
    xcb_generic_error_t *err;
    xcb_void_cookie_t cookie;

    
    xcb_window_t none = { XCB_NONE };

    cookie = xcb_test_fake_input( context->conn, XCB_KEY_PRESS, code, 
                                XCB_CURRENT_TIME, none, 0, 0, 0 );  
    xcb_test_fake_input( context->conn, XCB_KEY_RELEASE, code, 
                                XCB_CURRENT_TIME, none, 0, 0, 0 );
        
    err = xcb_request_check(context->conn, cookie);
    if (err)
    {
        printf("err ");
        free(err);
    }	
    
    printf("xtoq.c received key - uint8_t '%i', from Mac window #%i to context.window %ld\n", code,  window, context.window);
}

void
dummy_xtoq_button_down (xtoq_context_t *context, long x, long y, int window, int button)
{
    //xcb_window_t none = { XCB_NONE };
    xcb_test_fake_input (context->conn, XCB_BUTTON_PRESS, 1, XCB_CURRENT_TIME,
                         context->parent, x, y, 0);
                         // x has to be translated (?in the view)
    xcb_test_fake_input (context->conn, XCB_BUTTON_RELEASE, 1, XCB_CURRENT_TIME,
                         context->parent, x, y, 0);
    
    printf("button down received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
}

/* SOURCE: http://i3-wm.sourcearchive.com/documentation/3.b/client_8c-source.html */
/*
void
xtoq_request_close(xtoq_context_t *context) {
    xcb_get_property_cookie_t cookie;
    xcb_get_wm_protocols_reply_t protocols;

    // remove node from context list
    if (context = _xtoq_get_context_node_by_window_id(context->window))
        context = _xtoq_remove_context_node(context->window);
    
    // kill using xcb_kill_client                              
    if (!xtoq_client_supports_protocol(context->conn, window, atoms[WM_DELETE_WINDOW])) {
        xcb_kill_client(context->conn, context->window);
            return;
    }
    // kill using WM_DELETE_WINDOW
    else if (context->wm_delete_flag == 1){
        xcb_client_message_event_t event;
        
        memset(&event, 0, sizeof(xcb_client_message_event_t));

        event.response_type = XCB_CLIENT_MESSAGE;
        event.window = context->window;
        event.type = atoms[WM_PROTOCOLS];
        event.format = 32;
        event.data.data32[0] = atoms[WM_DELETE_WINDOW];
        event.data.data32[1] = XCB_CURRENT_TIME;
        
        xcb_send_event(context->conn, false, context->window, XCB_EVENT_MASK_NO_EVENT, (char*)&ev);
        xcb_flush(context->conn);
        free(event);
    }
    return;
}
 
*/
/* SOURCE: http://i3-wm.sourcearchive.com/documentation/3.b/client_8c-source.html */

/*
static bool xtoq_client_supports_protocol(xtoq_context_t * context, xcb_atom_t atom) {
    xcb_get_property_cookie_t cookie;
    xcb_get_wm_protocols_reply_t protocols;
    bool result = false;
    
    cookie = xcb_get_wm_protocols_unchecked(context->conn, context->window, atoms[WM_PROTOCOLS]);
    if (xcb_get_wm_protocols_reply(context->conn, cookie, &protocols, NULL) != 1)
        return false;
    
    //Check if the client’s protocols have the requested atom sets
    for (uint32_t i = 0; i < protocols.atoms_len; i++)
        if (protocols.atoms[i] == atom)
            result = true;
    
    xcb_get_wm_protocols_reply_wipe(&protocols);
    
    return result;
}
*/


