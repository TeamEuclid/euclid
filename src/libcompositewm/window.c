/* Copyright (c) 2012 Benjamin Carr
 *
 * window.c
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

#include "xtoq.h"
#include "xtoq_internal.h"

/* Functions only used within this file */

/* Sets the WM_* properties we care about in context */
void
set_icccm_properties (xtoq_context_t *context);

/* Set the WM_NAME property in context */
void
set_wm_name_in_context (xtoq_context_t *context);

/* Find out of the WM_DELETE_WINDOW property is set */
void
set_wm_delete_win_in_context (xtoq_context_t *context);

/* Initialize damage on a window */
void
init_damage_on_window (xtoq_context_t *context);


/* Set window to the top of the stack */
void
xtoq_set_window_to_top(xtoq_context_t *context) {

    const static uint32_t values[] = { XCB_STACK_MODE_ABOVE };
    
    /* Move the window on the top of the stack */
    xcb_configure_window (context->conn, context->window, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

/* Set window to the bottom of the stack */
void
xtoq_set_window_to_bottom(xtoq_context_t *context) {
    
    const static uint32_t values[] = { XCB_STACK_MODE_BELOW };
    
    /* Move the window on the top of the stack */
    xcb_configure_window (context->conn, context->window, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

/* Set input focus to window */ 
void
xtoq_set_input_focus(xtoq_context_t *context) {
    
    // Test -- David
    xcb_get_input_focus_cookie_t cookie = xcb_get_input_focus(context->conn);
    xcb_get_input_focus_reply_t *reply = xcb_get_input_focus_reply(context->conn, cookie, NULL);
    printf("Focus was in window #%d, now in #%d (window.c)\n", reply->focus, context->window);
    free(reply);
    
    // End test -- David
    
    xcb_set_input_focus(context->conn, XCB_INPUT_FOCUS_PARENT, context->window, XCB_CURRENT_TIME);
	xcb_flush(context->conn);
}

xtoq_context_t *
_xtoq_window_created(xcb_connection_t * conn, xcb_map_request_event_t *event) {

    /* Check to see if the window is already created */
    if (_xtoq_get_context_node_by_window_id(event->window)) {
        return NULL;
	}
    
    /* allocate memory for new xtoq_context_t */
    xtoq_context_t *context = malloc(sizeof(xtoq_context_t));
    
    xcb_get_geometry_reply_t *geom;
    geom = _xtoq_get_window_geometry(conn, event->window);
    
    /* set any available values from xcb_create_notify_event_t object pointer
       and geom pointer */
    context->conn = conn;
    context->window = event->window;
    context->parent = event->parent;
    context->x = geom->x;
    context->y = geom->y;
    context->width = geom->width;
    context->height = geom->height;

    free (geom);

	/* Set the ICCCM properties we care about */
	set_icccm_properties(context);
    
    /* register for damage */
    init_damage_on_window(context);

    /* add context to context_list */
    context = _xtoq_add_context_t(context);
    
    return context;
}

xtoq_context_t *
_xtoq_destroy_window(xcb_destroy_notify_event_t *event) {
    
    xtoq_context_t *context = _xtoq_get_context_node_by_window_id(event->window);
    if (!context) {
		/* Window isn't being managed */
		return NULL;
	}

    /* Destroy the damage object associated with the window. */
    xcb_damage_destroy(context->conn,context->damage);
    
    /* Call the remove function in context_list.c */
    _xtoq_remove_context_node(context->window);
    
    /* Return the pointer for the context that was removed from the list. */
    return context;
}
void
xtoq_configure_window(xtoq_context_t *context, int x, int y, int height, int width) {
    
    /* Set values for xtoq_context_t */
    context->x = x;
    context->y = y;
    context->width = width;
    context->height = height;
    
    uint32_t values[] = {(uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height };
    
    xcb_configure_window (context->conn, context->window, XCB_CONFIG_WINDOW_X 
                          | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);    
    xcb_flush(context->conn);
    return;
}

void
xtoq_request_close(xtoq_context_t *context) {
    
    /* check to see if the context is in the list */
    context = _xtoq_get_context_node_by_window_id(context->window);
    if (!context)
        return;
    
    /* kill using xcb_kill_client */                              
    if (!context->wm_delete_set == 1) {
        xcb_kill_client(context->conn, context->window);
        xcb_flush(context->conn);
        return;
    }
    /* kill using WM_DELETE_WINDOW */
    if (context->wm_delete_set == 1){
        xcb_client_message_event_t event;
        
        memset(&event, 0, sizeof(xcb_client_message_event_t));
        
        event.response_type = XCB_CLIENT_MESSAGE;
        event.window = context->window;
        event.type = _wm_atoms->wm_protocols_atom;
        event.format = 32;
        event.data.data32[0] = _wm_atoms->wm_delete_window_atom;
        event.data.data32[1] = XCB_CURRENT_TIME;
        
        xcb_send_event(context->conn, 0, context->window, XCB_EVENT_MASK_NO_EVENT, 
                       (char*)&event);
        xcb_flush(context->conn);
        return;
        
    }
    return;
}

/* Resize the window on server side */
void
_xtoq_resize_window (xcb_connection_t *conn, xcb_window_t window,
					 int width, int height)
{
	uint32_t values[2] = { width, height };

	xcb_configure_window(conn,
						 window,
						 XCB_CONFIG_WINDOW_WIDTH |
						 XCB_CONFIG_WINDOW_HEIGHT,
						 values);
	xcb_flush(conn);
}

void
_xtoq_map_window (xtoq_context_t *context)
{
	/* Map the window. May want to handle other things here */
	xcb_map_window(context->conn, context->window);
	xcb_flush(context->conn);
}

void
set_icccm_properties (xtoq_context_t *context)
{
	set_wm_name_in_context(context);
	set_wm_delete_win_in_context(context);
}

void
set_wm_name_in_context (xtoq_context_t *context)
{
	xcb_get_property_cookie_t cookie;
	xcb_get_property_reply_t *reply;
	xcb_generic_error_t *error;
	char *value;
	int length;

	cookie = xcb_get_property(context->conn,
							  0,
							  context->window,
							  XCB_ATOM_WM_NAME,
							  XCB_GET_PROPERTY_TYPE_ANY,
							  0,
							  128);
	reply = xcb_get_property_reply(context->conn,
								   cookie,
								   &error);
	if (!reply) {
		context->name = NULL;
		return;
	}
	length = xcb_get_property_value_length(reply);
	value = (char *) xcb_get_property_value(reply);

	context->name = malloc(sizeof(char) * (length + 1));
	strncpy(context->name, value, length);
	context->name[length] = '\0';
}

void
set_wm_delete_win_in_context (xtoq_context_t *context)
{
	xcb_get_property_cookie_t cookie;
	xcb_get_property_reply_t *reply;
	xcb_atom_t *prop_atoms;
	int prop_length;
	xcb_generic_error_t *error;
    int i;

	/* Get the WM_PROTOCOLS */
	cookie = xcb_get_property(context->conn,
							  0,
							  context->window,
							  _wm_atoms->wm_protocols_atom,
							  XCB_ATOM_ATOM,
							  0,
							  UINT_MAX);

	reply = xcb_get_property_reply(context->conn,
								   cookie,
								   &error);

	if (!reply) {
		context->wm_delete_set = 0;
		return;
	}
	prop_length = xcb_get_property_value_length(reply);
	prop_atoms = (xcb_atom_t *) xcb_get_property_value(reply);
	free(reply);

    /* See if the WM_DELETE_WINDOW is in WM_PROTOCOLS */
    for (i = 0; i < prop_length; i++) {
        if (prop_atoms[i] == _wm_atoms->wm_delete_window_atom) {
            context->wm_delete_set = 1;
            return;
        }
    }
    context->wm_delete_set = 0;

	return;
}

void
init_damage_on_window (xtoq_context_t *context)
{
	xcb_damage_damage_t damage_id;
	uint8_t level;
    xcb_void_cookie_t cookie;

	damage_id = xcb_generate_id(context->conn);
    
    // Refer to the Damage Protocol. level = 0 corresponds to the level
    // DamageReportRawRectangles.  Another level may be more appropriate.
    level = XCB_DAMAGE_REPORT_LEVEL_BOUNDING_BOX;
    cookie = xcb_damage_create(context->conn,
							   damage_id,
							   context->window,
							   level);
    
	if (_xtoq_request_check(context->conn, cookie,
							"Could not create damage for window")) {
		context->damage = 0;
		return;
	}
    /* Assign this damage object to the roots window's context */
    context->damage = damage_id;
}


