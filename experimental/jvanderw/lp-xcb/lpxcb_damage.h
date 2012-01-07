/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_damage.h
 */

#ifndef _LPXCB_DAMAGE_H_
#define _LPXCB_DAMAGE_H_

#include <xcb/xcb.h>
#include "lpxcb_api.h"
#include "lpxcb_table.h"
#include "lpxcb_util.h"

/**
 * Find the next damaged window
 */
lpxcb_window_t *
lpxcb_find_damaged (xcb_connection_t *conn);

/**
 * Add damage to the given window
 */
void
lpxcb_damage_window(lpxcb_window_t *lpxcb_window,
                    int x, int y,
                    int width, int height);

/**
 * Remove damage from the given window
 */
void
lpxcb_remove_damage(lpxcb_window_t *lpxcb_window);

/**
 * Update the damage of the window
 */
void
lpxcb_update_window(lpxcb_window_t *lpxcb_window);

#endif  /* _LPXCB_DAMAGE_H_ */
