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

xtoq_context_t *
_xtoq_window_created(xcb_connection_t * conn, xcb_create_notify_event_t *event) {

    // Check to see if the window is already created
    if (_xtoq_get_context_node_by_window_id(event->window))
        return NULL;
    
    // allocate memory for new xtoq_context_t
    xtoq_context_t *context = malloc(sizeof(xtoq_context_t));
    
    // set any available values from xcb_create_notify_event_t object pointer
    context->conn = conn;
    context->window = event->window;
    context->parent = event->parent;
    context->x = event->x;
    context->y = event->y;
    context->width = event->width;
    context->height = event->height;
    
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
    xcb_damage_destroy(context->conn,context->damage);	
    
    // Call the remove function in context_list.c
    _xtoq_remove_context_node(context->window);
    
    //Returns the pointer for the context that was removed from the list.
    return context;
}






