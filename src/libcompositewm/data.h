/* Copyright (c) 2012 Jess VanDerwalker <washu@sonic.net>
 * All rights reserved.
 *
 * data.h
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


#ifndef _DATA_H_
#define _DATA_H_

#include <xcb/xcb.h>
#include <xcb/damage.h>

#define XTOQ_DAMAGE 0
#define XTOQ_EXPOSE 1
#define XTOQ_CREATE 2
#define XTOQ_DESTROY 3

typedef struct xtoq_context_t {
    xcb_connection_t *conn;
    xcb_drawable_t window;
    xcb_window_t parent;
    xcb_damage_damage_t damage;
    int x;
    int y;
    int width;
    int height;
    int damaged_x;
    int damaged_y;
    int damaged_width;
    int damaged_height;
	char *name;					/* The name of the window */
	int wm_delete_set;			/* Flag for WM_DELETE_WINDOW, 1 if set */
    void *local_data;   // Area for data client cares about
} xtoq_context_t;

typedef struct xtoq_event_t {
    xtoq_context_t *context;
    int event_type;
} xtoq_event_t;

typedef struct image_data_t {
    uint8_t *data;
    int length;
}  image_data_t;

typedef struct xtoq_image_t {
    xcb_image_t *image;
    int x;
    int y;
    int width;
    int height;
} xtoq_image_t;

typedef void (*xtoq_event_cb_t) (xtoq_event_t const *event);

extern int _damage_event;

#endif
