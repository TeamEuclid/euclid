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

void _xtoq_init_list(){
    _xtoq_window_list.head = NULL;
    _xtoq_window_list.count = 0;
}
/*
void 
testList(){
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
    _xtoq_add_context_t(init_reply);

}
*/

void
_xtoq_add_context_t(xtoq_context_t context) {
  /*  if (list->head == NULL) {
        list->head = node;
        node->next = NULL;
    }
    else {
        struct context_node * temp = list->head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = node;
        node->next = NULL;
    } */
}

void
_xtoq_remove_context_node(xcb_window_t window_id) {
 /*   if (list->head == NULL) {
        //error empty list
        printf("Error, empty list\n");
    }
    struct context_node * curr= list->head, * prev = NULL;
    if (curr->context->window == window_id) {
        list->head = curr->next;
        return;
    }
    while (curr->next != NULL) {
        curr = curr->next;
        if (curr->context->window == window_id) {
            prev->next = curr->next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    // error, no node with window_id
    //printf("Error, no node with window_id: %d", (int)window_id);
  */
}

_xtoq_context_node *
_xtoq_get_context_node_by_window_id (xcb_window_t window_id) {
/*    if (list->head == NULL) {
        //error, empty list
        printf("Error, empty list\n");
    }
    struct context_node * temp = list->head;
    while (temp->next != NULL) {
        if (temp->context->window == window_id)
            return temp;
        temp = temp->next;
    }
    // error, no window with that id
    // printf("Error, no node with window_id: %d", (int)window_id);
    return NULL;
 */
}

/* this will surely break: */
/*
context_list * get_context_node(context_list * list, int index) {
  context_node * temp = list
  int i = 0;
  while (i < index) {
    temp = temp->next;
  }
  return temp;
}
*/


