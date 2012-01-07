/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb.h
 */

#ifndef _LPXCB_API_H_
#define _LPXCB_API_H_

#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/damage.h>
#include <xcb/composite.h>
#include <xcb/xfixes.h>
#include "lpxcb_event.h"

/* Holds information for a window */
typedef struct lpxcb_window_t {
    /* The connection associated with this window */
    xcb_connection_t *conn;
    /* The id of this window */
    xcb_drawable_t window;
    /* The id of this parent's window, if it has one. */
    xcb_drawable_t parent;
    /* Rectangle used to manage damage */
    xcb_rectangle_t damage_rect;
    /* Damage associated with this window */
    xcb_damage_damage_t damage;
    /* Region of window marked as damage area */
    xcb_xfixes_region_t region;
    xcb_xfixes_region_t repair;
    /* The next window with damage */
    struct lpxcb_window_t *next_damaged;
    /* Should we have something for the pixmap? */
} lpxcb_window_t;


/**
 * Return an lpxcb_window_t if there is damage, otherwise 0
 */
lpxcb_window_t *
lpxcb_check_window (xcb_connection_t *conn, xcb_window_t window);

#endif  /* _LPXCB_API_H_ */
