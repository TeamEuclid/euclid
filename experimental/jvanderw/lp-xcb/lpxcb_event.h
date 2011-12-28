/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_event.h
 */

#ifndef _LPXCB_EVENT_H_
#define _LPXCB_EVENT_H_


#include <xcb/xcb.h>
#include "lpxcb_api.h"

/**
 * Handle all the pending events for the given connection
 */
void
lpxcb_handle_event (xcb_connection_t *conn);

/**
 *
 */


#endif  /* _LPXCB_EVENT_H_ */
