/* Copyright (c) 2012 Benjamin Carr
 *
 * context_list.c
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

#include "context_list.h"
/*
void _xtoq_init_list(){
    _xtoq_window_list.head = NULL;
    _xtoq_window_list.count = 0;
}*/


void 
testList(){ /*
    xcb_connection_t *conn;
    int conn_screen;
    xcb_screen_t *root_screen;
    xcb_drawable_t root_window;
    
    xcb_get_geometry_reply_t *geom_reply;
    
    conn = xcb_connect(":1", &conn_screen);
    
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;
    
    // Get the geometry of the root window 
    geom_reply = _xtoq_get_window_geometry(conn, root_window);
    
    //WriteWindowInfo(conn, root_window);
	//WriteAllChildrenWindowInfo(conn, root_window);
    
	xcb_flush(conn);
    
    xtoq_context_t init_reply;
    init_reply.conn = conn;
    init_reply.window = root_window;
    
    // not sure about this error ...
    //_xtoq_add_context(init_reply);
*/
    xtoq_context_t ctxt1;
    xtoq_context_t ctxt2;
    xtoq_context_t ctxt3;
    ctxt1.window = 1;
    ctxt2.window = 2;
    ctxt3.window = 3;
    _xtoq_add_context_t(ctxt1);
    _xtoq_add_context_t(ctxt2);
    _xtoq_add_context_t(ctxt3);
    
}


void
_xtoq_add_context_t(struct xtoq_context_t context)
{
    _xtoq_context_node *new;
    _xtoq_context_node *curr;
    _xtoq_context_node *prev;
    xtoq_context_t hmm;
    
    /* Does the window already exist */
    if (_xtoq_get_context_node_by_window_id(context.window))
        return;
    
    /* Create node to hold the new window */

    /* NOTE: There are two things that you need to allocate memory for
     * - the xtoq_context AND the _xtoq_context_node that you are
     * going to put the xtoq_context into. */
    new = malloc(sizeof(xtoq_context_t));
    if (!new) {
        exit(1);                /* Should we handle this differently? */
    }
    /* NOTE: I don't think this will work since context may go out of
     * scope when the function ends - might have to make a deep
     * copy */
    new->context = &context;
    hmm = context;
    
    /* Handle the case where this is the first node added */
    if (!_xtoq_window_list_head) {
        new->prev = NULL;
        new->next = NULL;
        _xtoq_window_list_head = new;
    } else {
        curr = _xtoq_window_list_head;
        while (curr->next) {
            prev = curr;
            curr = curr->next;
        }
        curr->next = new;
        new->prev = curr;
        new->next = NULL;
    }
    
    
    return;
}       

_xtoq_context_node *
_xtoq_get_context_node_by_window_id (xcb_window_t window_id)
{
    _xtoq_context_node *curr;
    
    if (_xtoq_window_list_head) {
        return NULL;
    }
    
    curr = _xtoq_window_list_head;
    while (curr) {
        if (curr->context->window == window_id) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


void
_xtoq_remove_context_node(xcb_window_t window_id) {

    _xtoq_context_node *curr;
    
    curr = _xtoq_window_list_head;
    while (curr) {
        if (curr->context->window == window_id) {
            curr->next->prev = curr->prev;
            if (curr->prev) {
                curr->prev->next = curr->next;
            }
            free(curr);
            return;
        }
    }
    return;
}
