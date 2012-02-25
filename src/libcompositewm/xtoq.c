/* Copyright (c) 2012 David Snyder, Benjamin Carr, Braden Wooley
 *
 * rootimg_api.h
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

/* #ifndef _XTOQ_C_ */
/* #define _XTOQ_C_ */

#include "xtoq.h"
#include "xtoq_internal.h"
#include <string.h>

// aaron key stuff
#define XK_Shift_L                       0xffe1
xcb_key_symbols_t *syms = NULL;
// end aaron key stuff
xtoq_context_t *root_context = NULL;

// This init function needs set the window to be registered for events!
// First one we should handle is damage
xtoq_context_t *
xtoq_init(char *screen) {
    xcb_connection_t *conn;
    int conn_screen;
    xcb_screen_t *root_screen;
    xcb_drawable_t root_window;
    uint32_t mask_values[1];
 
    conn = xcb_connect(screen, &conn_screen);
    
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;
    
    // Set the mask for the root window so we know when new windows
    // are created on the root. This is where we add masks for the events
    // we care about catching on the root window.
    mask_values[0] = XCB_EVENT_MASK_KEY_PRESS |
                     XCB_EVENT_MASK_BUTTON_PRESS |
                     XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    xcb_change_window_attributes (conn, root_window,
                                  XCB_CW_EVENT_MASK, mask_values);

	xcb_flush(conn);
    
    // TODO: Creating this reference to the root window so there is a way
    // to get at it from other functions - since we don't have a data
    // structure for the windows yet. This should be deleted later
    // because this is getting VERY HACKY
    root_context = malloc(sizeof(xtoq_context_t));
    root_context->conn = conn;
    root_context->parent = 0;
    root_context->window = root_window;
    
    // Set width, height, x, & y from root_screen into the xtoq_context_t
    root_context->width = root_screen->width_in_pixels;
    root_context->height = root_screen->height_in_pixels;
    root_context->x = 0;
    root_context->y = 0;
    // TODO END
    
    xtoq_context_t init_reply;
    init_reply.conn = conn;
    init_reply.window = root_window;
    
    // Set width, height, x, & y from root_screen into the xtoq_context_t
    init_reply.width = root_screen->width_in_pixels;
    init_reply.height = root_screen->height_in_pixels;
    init_reply.x = 0;
    init_reply.y = 0;    
    
    // TODO: May want to send &init_reply instead of root window
    _xtoq_init_damage(root_context);
    
    _xtoq_init_xfixes(root_context);
    
    _xtoq_add_context_t(&init_reply);
        
    syms = xcb_key_symbols_alloc(conn);
    _xtoq_init_extension(conn, "XTEST");
	_xtoq_init_extension(conn, "XKEYBOARD");
    
    return root_context;
}

xtoq_image_t *
xtoq_get_image(xtoq_context_t *context) {
    
    xcb_get_geometry_reply_t *geom_reply;
    
    //image_data_t img_data;
    xcb_image_t *image;
    
    geom_reply = _xtoq_get_window_geometry(context->conn, context->window);
    
    xtoq_image_t * xtoq_image = (xtoq_image_t *) malloc(10 * sizeof (xtoq_image_t));
    
	//xcb_flush(context.conn);
    /* Get the image of the root window */
    image = xcb_image_get(context->conn,
                          context->window,
                          geom_reply->x,
                          geom_reply->y,
                          geom_reply->width,
                          geom_reply->height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    
    xtoq_image->image = image;
    xtoq_image->x = geom_reply->x;
    xtoq_image->y = geom_reply->y;
    xtoq_image->width = geom_reply->width;
    xtoq_image->height = geom_reply->height;
    
    free(geom_reply);
    return xtoq_image;
}

void
xtoq_free_image(xcb_image_t *img) {
    free(img);
}



xtoq_event_t
dummy_xtoq_wait_for_event(xtoq_context_t context) {
    
    sleep(4);
    xtoq_event_t event;
    xtoq_context_t new_context;
    new_context.window = context.window;
    new_context.conn = context.conn;
    event.context = &new_context;
    event.event_type = XTOQ_DAMAGE;
    
    return event;
}

int 
xtoq_start_event_loop (xtoq_context_t *root_context, void *callback)
{
	/* Simply call our internal function to do the actual setup */
	return _xtoq_start_event_loop(root_context->conn, callback);
}

xtoq_image_t *
test_xtoq_get_image(xtoq_context_t *context) {
    
   // printf("Top of test get image\n");
    //xcb_get_geometry_reply_t *geom_reply;
    
    //image_data_t img_data;
    xcb_image_t *image;
    
    //geom_reply = _xtoq_get_window_geometry(context.conn, context.window);
    
	//xcb_flush(context.conn);
    /* Get the image of the root window */
    image = xcb_image_get(context->conn,
                          context->window,
                          context->damaged_x,
                          context->damaged_y,
                          context->damaged_width,
                          context->damaged_height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    //xtoq_image_t * xtoq_image;
    
    //FIXME - Calculate memory size correctly
    xtoq_image_t * xtoq_image = (xtoq_image_t *) malloc(10 * sizeof (xtoq_image_t));
    
    xtoq_image->image = image;
    xtoq_image->x = context->damaged_x;
    xtoq_image->y = context->damaged_y;
    xtoq_image->width = context->damaged_width;
    xtoq_image->height = context->damaged_height;
    
    //printf("Returning image with x=%d y=%d w=%d h=%d\n", xtoq_image->x, xtoq_image->y, xtoq_image->width, xtoq_image->height);
 
    //free(geom_reply);
    return xtoq_image;
}


void 
xtoq_image_destroy(xtoq_image_t * xtoq_image){
    //FIXME - is this all that needs to be done?
    xcb_image_destroy(xtoq_image->image);
    free(xtoq_image);
}
 
void
dummy_xtoq_key_press (xtoq_context_t *context, int window, uint8_t code)
{
    xcb_generic_error_t *err;
    xcb_void_cookie_t cookie;
    xcb_window_t none = { XCB_NONE };
    
    // context->window 

    cookie = xcb_test_fake_input( context->conn, XCB_KEY_PRESS, code, 
                                XCB_CURRENT_TIME, none, 0, 0, 1 );  
    xcb_test_fake_input( context->conn, XCB_KEY_RELEASE, code, 
                                XCB_CURRENT_TIME, none,0 ,0 , 1 );
        
    err = xcb_request_check(context->conn, cookie);
    if (err)
    {
        printf("err ");
        free(err);
    }	
    
    printf("xtoq.c received key - uint8_t '%i', from Mac window #%i to context.window %ld\n", code,  window, context->window);
}

void
dummy_xtoq_button_down (xtoq_context_t *context, long x, long y, int window, int button)
{
    //xcb_window_t none = { XCB_NONE };
    xcb_test_fake_input (context->conn, XCB_BUTTON_PRESS, 1, XCB_CURRENT_TIME,
                         context->parent, x, y, 0);
                         // x has to be translated (?in the view)
    xcb_test_fake_input (context->conn, XCB_BUTTON_RELEASE, 1, XCB_CURRENT_TIME,
                         context->parent, x, y, 0);
    
    printf("button down received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
}

