/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_util.h
 */

#ifndef _LPXCB_UTIL_H_
#define _LPXCB_UTIL_H_

#include <xcb/xcb.h>
#include <xcb/xfixes.h>

/**
 * Check a request with given cookie and return the error code if
 * there was one.
 */
int
lpxcb_check_request (xcb_connection_t *conn,
                     xcb_void_cookie_t cookie,
                     char *msg);

/**
 * Get geometry for the given window. The returned structure must be
 * freed by the caller.
 */
xcb_get_geometry_reply_t *
lpxcb_get_window_geometry (xcb_connection_t *conn, xcb_window_t window);

/**
 * Set the x, y, width, height, and border width for a given window.
 */
void
lpxcb_set_window_dimensions (xcb_connection_t *conn,
                             xcb_window_t window,
                             uint16_t x,
                             uint16_t y,
                             uint16_t height,
                             uint16_t width,
                             uint16_t border_width);


/**
 * Get the window attributes for the given window. Retuned structure
 * must be freed by the caller
 */
xcb_get_window_attributes_reply_t *
lpxcb_get_window_attrs (xcb_connection_t *conn, xcb_window_t window);

/**
 * Get the array of rectangles from a given region. This is just for debugging.
 */
void
lpxcb_get_region_rects (xcb_connection_t *conn, xcb_xfixes_region_t region);

#endif  /* _LPXCB_UTIL_H_ */
