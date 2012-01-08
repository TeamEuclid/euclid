/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb.h
 */

#ifndef _LPXCB_API_H_
#define _LPXCB_API_H_

#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>

#include "lpxcb_data.h"
#include "lpxcb_damage.h"
#include "lpxcb_event.h"



/**
 * Return an lpxcb_window_t if there is damage, otherwise 0
 */
lpxcb_window_t *
lpxcb_check_window (xcb_connection_t *conn, xcb_window_t window);

#endif  /* _LPXCB_API_H_ */
