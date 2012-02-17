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

#include "xtoq_internal.h"

_xtoq_context_node *_xtoq_window_list_head = NULL;

xtoq_context_t *
_xtoq_add_context_t(struct xtoq_context_t *context)
{
    /* Does the window already exist */
    /*
    if (_xtoq_get_context_node_by_window_id(context->window))
        return NULL;
    
    xtoq_context_t * new_context;
    new_context = malloc(sizeof(xtoq_context_t));
    
    new_context->conn = context->conn;
    new_context->window = context->window;
    new_context->parent = context->parent;
    new_context->damage = context->damage;
    new_context->x = context->x;
    new_context->y = context->y;
    new_context->width = context->width;
    new_context->height = context->height;
    new_context->local_data = context->local_data;
     */
    _xtoq_context_node *new_node;
    _xtoq_context_node *curr;
    _xtoq_context_node *prev;
    
    
    /* Create node to hold the new window */
    
    new_node = malloc(sizeof(_xtoq_context_node));
    if (!new_node) {
        exit(1);
    }
    new_node->context = context;
    //new_node->context = new_context;
    
    /* Handle the case where this is the first node added */
    if (!_xtoq_window_list_head) {
        new_node->prev = NULL;
        new_node->next = NULL;
        _xtoq_window_list_head = new_node;
    } else {
        curr = _xtoq_window_list_head;
        while (curr->next) {
            prev = curr;
            curr = curr->next;
        }
        curr->next = new_node;
        new_node->prev = curr;
        new_node->next = NULL;
    }
    
    
    return new_node->context;
}       

_xtoq_context_node *
_xtoq_get_context_node_by_window_id (xcb_window_t window_id)
{
    _xtoq_context_node *curr;
        
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
            free(curr->context);
            if(curr->next){
                curr->next->prev = curr->prev;
            }
            if (curr->prev) {
                curr->prev->next = curr->next;
            }
            else{
                _xtoq_window_list_head = curr->next;
            }
                
            free(curr);
            return;
        }
        else
            curr = curr->next;
    }
    return;
}
