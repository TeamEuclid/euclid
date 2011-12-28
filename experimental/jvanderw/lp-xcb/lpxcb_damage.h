/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_damage.h
 */

#ifndef _LPXCB_DAMAGE_H_
#define _LPXCB_DAMAGE_H_

#include <xcb/xcb.h>
#include "lpxcb_api.h"

lpxcb_window_t *
lpxcb_find_damaged (xcb_connection_t *conn);

#endif  /* _LPXCB_DAMAGE_H_ */
