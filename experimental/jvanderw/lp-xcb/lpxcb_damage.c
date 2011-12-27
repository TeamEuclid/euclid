/* Copyright (c) 2011 Jess VanDerwalker
 *
 * lpxcb_damage.c
 */

#include "lpxcb_damage.h"
#include "lpxcb_table.h"


lpxcb_window_t *
lpxcb_find_damaged (xcb_connection_t *conn)
{
    lpxcb_connection_t *lpxcb_conn;

    lpxcb_conn = lpxcb_find_connection(conn);
    return lpxcb_conn->damaged;
}
