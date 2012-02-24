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

/* Set window to the top of the stack */
void
xtoq_set_window_to_top(xtoq_context_t *context) {
    
    const static uint32_t values[] = { XCB_STACK_MODE_ABOVE };
    
    /* Move the window on the top of the stack */
    xcb_configure_window (context->conn, context->window, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

/* Set input focus to window */ 
void
xtoq_set_input_focus(xtoq_context_t *context) {
    
   // values[0] = CHILD_EVENT_MASK & ~(XCB_EVENT_MASK_FOCUS_CHANGE);
   // xcb_change_window_attributes(context->conn, context->window, XCB_CW_EVENT_MASK, values);
                                     // or is it XCB_INPUT_FOCUS_PARENT
    xcb_set_input_focus(context->conn, XCB_INPUT_FOCUS_POINTER_ROOT, context->window, XCB_CURRENT_TIME);

    //values[0] = CHILD_EVENT_MASK;
    //xcb_change_window_attributes(context->conn, context->window, XCB_CW_EVENT_MASK, values);
}

/* Set window to the bottom of the stack */
void
xtoq_set_window_to_bottom(xtoq_context_t *context) {
    
    const static uint32_t values[] = { XCB_STACK_MODE_BELOW };
    
    /* Move the window on the top of the stack */
    xcb_configure_window (context->conn, context->window, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

xtoq_context_t *
_xtoq_window_created(xcb_connection_t * conn, xcb_create_notify_event_t *event) {

    // Check to see if the window is already created
    if (_xtoq_get_context_node_by_window_id(event->window))
        return NULL;
    
    // allocate memory for new xtoq_context_t
    xtoq_context_t *context = malloc(sizeof(xtoq_context_t));
    
    xcb_get_geometry_reply_t *geom;
    geom = _xtoq_get_window_geometry(conn, event->window);
    
    // set any available values from xcb_create_notify_event_t object pointer
    // and geom pointer
    context->conn = conn;
    context->window = event->window;
    context->parent = event->parent;
    context->x = geom->x;
    context->y = geom->y;
    context->width = geom->width;
    context->height = geom->height;

    // done with geom
    free (geom);

	/* Set the ICCCM properties we care about */
	set_icccm_properties(context);
    
    //register for damage
    _xtoq_init_damage(context);

    // add context to context_list
    context = _xtoq_add_context_t(context);
    
    return context;
}

xtoq_context_t * _xtoq_destroy_window(xcb_destroy_notify_event_t *event) {
    
  //  if (_xtoq_get_context_node_by_window_id(event->window) == NULL)
    //    return NULL;
    
    xtoq_context_t *context = _xtoq_get_context_node_by_window_id(event->window);
    
    // Destroy the damage object associated with the window.
    // TODO: I'm not sure if this frees the damage object...
    xcb_damage_destroy(context->conn,context->damage);
    
    // Call the remove function in context_list.c
    _xtoq_remove_context_node(context->window);
    
    //Returns the pointer for the context that was removed from the list.
    return context;
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

	context->name = strdup(value);
}

void
set_wm_delete_win_in_context (xtoq_context_t *context)
{
	xcb_atom_t wm_delete_atom;
	xcb_atom_t wm_protocols_atom;
	xcb_intern_atom_reply_t *atom_reply;
	xcb_intern_atom_cookie_t atom_cookie;
	xcb_get_property_cookie_t cookie;
	xcb_get_property_reply_t *reply;
	xcb_atom_t *prop_atoms;
	int prop_length;
	xcb_generic_error_t *error;

	/* We need to get some atoms first - this may need to be done
	*  in the init */
	/* WM_PROTOCOLS atom */
	atom_cookie = xcb_intern_atom(context->conn,
								  0,
								  12,
								  "WM_PROTOCOLS");
	atom_reply = xcb_intern_atom_reply(context->conn,
									   atom_cookie,
									   NULL);
	assert(atom_reply);
	wm_protocols_atom = atom_reply->atom;
	free(atom_reply);

	/* WM_DELETE_WINDOW atom */
	atom_cookie = xcb_intern_atom(context->conn,
								  0,
								  16,
								  "WM_DELETE_WINDOW");
	atom_reply = xcb_intern_atom_reply(context->conn,
									   atom_cookie,
									   NULL);
	assert(atom_reply);
	wm_delete_atom = atom_reply->atom;
	free(atom_reply);

	/* Get the WM_PROTOCOLS */
	cookie = xcb_get_property(context->conn,
							  0,
							  context->window,
							  wm_protocols_atom,
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
	return;
}
