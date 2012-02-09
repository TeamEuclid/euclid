/* Copyright (c) 2012 Benjamin Carr
 *
 * context_list.h
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


#ifndef _CONTEXT_LIST_H_
#define _CONTEXT_LIST_H_

#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include "xtoq.h"
#include "util.h"

typedef struct _xtoq_context_node {
    xtoq_context_t *context;
    struct _xtoq_context_node * next;
    struct _xtoq_context_node * prev;
} _xtoq_context_node;

typedef struct _xtoq_context_list {
    struct _xtoq_context_node * head;
    int count;
} _xtoq_context_list;

/* this is the head pointer */
_xtoq_context_node *_xtoq_window_list_head;

/* NOTE: Added this because _xtoq_init_list seems to need
 * it. _xtoq_window_list might be a bad name since it prettly close to
 * the names of the structures your using here */
_xtoq_context_list _xtoq_window_list ;

void _xtoq_init_list();


void 
testList();


// _xtoq_add_context_t should be taking a context_t
void
_xtoq_add_context_t(xtoq_context_t context);
 
void
_xtoq_remove_context_node(xcb_window_t window_id);

_xtoq_context_node *
_xtoq_get_context_node_by_window_id (xcb_window_t window_id);


#endif
