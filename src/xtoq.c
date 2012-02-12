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
#include <string.h>

// TODO: Decide where this variable needs to live.
int _damage_event;

// aaron key stuff
#define XK_Shift_L                       0xffe1
xcb_key_symbols_t *syms = NULL;
// end aaron key stuff

// This init function needs set the window to be registered for events!
// First one we should handle is damage
xtoq_context_t
xtoq_init(char *screen) {
    xcb_connection_t *conn;
    int conn_screen;
    xcb_screen_t *root_screen;
    xcb_drawable_t root_window;
    uint32_t mask_values[1];
    
    xcb_get_geometry_reply_t *geom_reply;
 
    conn = xcb_connect(screen, &conn_screen);
    
    root_screen = xcb_aux_get_screen(conn, conn_screen);
    root_window = root_screen->root;
    
    
    //xtoq_context_t contxt;
    //contxt.window = root_window;
    //contxt.conn = conn;
    //_xtoq_init_damage(contxt);
    
    // Set the mask for the root window so we know when new windows
    // are created on the root. This is where we add masks for the events
    // we care about catching on the root window.
    mask_values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    xcb_change_window_attributes (conn, root_window,
                                  XCB_CW_EVENT_MASK, mask_values);

    
    /* Get the geometry of the root window */
    geom_reply = _xtoq_get_window_geometry(conn, root_window);
    
    //WriteWindowInfo(conn, root_window);
	//WriteAllChildrenWindowInfo(conn, root_window);
    
    

    
	xcb_flush(conn);
    
    xtoq_context_t init_reply;
    init_reply.conn = conn;
    init_reply.window = root_window;
    
    // Set width, height, x, & y from root_screen into the xtoq_context_t
    init_reply.width = root_screen->width_in_pixels;
    init_reply.height = root_screen->height_in_pixels;
    init_reply.x = 140;
    init_reply.y = 42;
    
    
    _xtoq_init_damage(init_reply);
    
    // not sure about this error ...
    //_xtoq_add_context_t(init_reply);
    
    return init_reply;
}

xcb_query_extension_reply_t * _xtoq_init_extension(xcb_connection_t *conn, char *extension_name) {
    xcb_query_extension_cookie_t cookie = xcb_query_extension(conn, strlen(extension_name), extension_name);
	xcb_query_extension_reply_t *reply = xcb_query_extension_reply(conn, cookie, NULL);
	if (!reply->present) {
		free(reply);
        printf("%s extension not present", extension_name);
        exit(1);
	} else {
        printf("%s extension present", extension_name);
    }
    
	return reply;
}

void _xtoq_init_damage(xtoq_context_t contxt) {
    
    xcb_query_extension_reply_t *reply =_xtoq_init_extension(contxt.conn, "DAMAGE");
    
    xcb_damage_query_version_cookie_t version_cookie = 
    xcb_damage_query_version(contxt.conn, 
                             XCB_DAMAGE_MAJOR_VERSION,
                             XCB_DAMAGE_MINOR_VERSION);
	xcb_damage_query_version_reply_t* version_reply = xcb_damage_query_version_reply(contxt.conn, version_cookie, NULL);

	free(version_reply);
	free(reply);
    xcb_damage_damage_t damage = xcb_generate_id(contxt.conn);
    
    // Refer to the Damage Protocol. level = 0 corresponds to the level
    // DamageReportRawRectangles.  Another level may be more appropriate.
    uint8_t level = 0;
    xcb_void_cookie_t v = xcb_damage_create(contxt.conn,
                                        damage, contxt.window, level);

    xcb_query_extension_reply_t *damage_extension = _xtoq_init_extension(contxt.conn, "DAMAGE");
    _damage_event = damage_extension->first_event + XCB_DAMAGE_NOTIFY;
    free(damage_extension);	
}


xcb_image_t *
xtoq_get_image(xtoq_context_t context) {
    
    xcb_get_geometry_reply_t *geom_reply;
    
    //image_data_t img_data;
    xcb_image_t *image;
    
    geom_reply = _xtoq_get_window_geometry(context.conn, context.window);
    
    //WriteWindowInfo(context.conn, context.window);
	//WriteAllChildrenWindowInfo(context.conn, context.window);
    //img_data = GetWindowImageData(context.conn, context.window);
    
	xcb_flush(context.conn);
    /* Get the image of the root window */
    image = xcb_image_get(context.conn,
                          context.window,
                          geom_reply->x,
                          geom_reply->y,
                          geom_reply->width,
                          geom_reply->height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    free(geom_reply);
    return image;
}

int
dummy_xtoq_get_image(xtoq_context_t context) {
    
    // TEMP until we can return an image
    xcb_connection_t *conn_two;
    int conn_two_screen;
    xcb_screen_t *root_two_screen;
    xcb_drawable_t root_two_window;
    xcb_drawable_t window;
    uint32_t mask;
    uint32_t values[1];
    xcb_void_cookie_t cookie;
    xcb_pixmap_t pixmap;
    xcb_gcontext_t gc;
    // END TEMP
    
    xcb_get_geometry_reply_t *geom_reply;
    
    image_data_t img_data;
    xcb_image_t *image;
    
    geom_reply = _xtoq_get_window_geometry(context.conn, context.window);
    
    _xtoq_write_window_info(context.conn, context.window);
	_xtoq_write_all_children_window_info(context.conn, context.window);
    img_data = _xtoq_get_window_image_data(context.conn, context.window);
    
	xcb_flush(context.conn);
    /* Get the image of the root window */
    image = xcb_image_get(context.conn,
                          context.window,
                          geom_reply->x,
                          geom_reply->y,
                          geom_reply->width,
                          geom_reply->height,
                          (unsigned int) ~0L,
                          XCB_IMAGE_FORMAT_Z_PIXMAP);
    /* Set up the events the window will recognize */
    mask = XCB_CW_EVENT_MASK;
    values[0] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
    
    /* Create our new window on the default display. Make it half the size */
    conn_two = xcb_connect(NULL, &conn_two_screen);
    root_two_screen = xcb_aux_get_screen(conn_two, conn_two_screen);
    root_two_window = root_two_screen->root;
    window = xcb_generate_id(conn_two);
    cookie = xcb_create_window_checked(conn_two,
                                       XCB_COPY_FROM_PARENT,
                                       window,
                                       root_two_window,
                                       geom_reply->x,
                                       geom_reply->y,
                                       geom_reply->width / 2,
                                       geom_reply->height / 2,
                                       geom_reply->border_width,
                                       XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                       root_two_screen->root_visual,
                                       mask,
                                       values);
    if (_xtoq_request_check(conn_two, cookie, "Falied to create new window")) {
        exit(1);
    }
    
    _xtoq_write_window_info(conn_two, window);
    /* Map the window and flush the connection so it draws to the screen */
    xcb_map_window(conn_two, window);
    xcb_flush(conn_two);
    _xtoq_write_window_info(conn_two, window);
    
    /* Create the pixmap and associate it with our new window. */
    pixmap = xcb_generate_id(conn_two);
    cookie = xcb_create_pixmap(conn_two,
                               geom_reply->depth,
                               pixmap,
                               window,
                               geom_reply->width,
                               geom_reply->height);
    if (_xtoq_request_check(conn_two, cookie, "Failed to create pixmap")) {
        exit(1);
    }
    
    /* Put the root_window image into the pixmap. Note that a gc is
     * created, but I believe it is ignored. */
    gc = xcb_generate_id(conn_two);
    xcb_create_gc(conn_two, gc, window, 0, 0);
    cookie = xcb_image_put(conn_two,
                           pixmap,
                           gc,
                           image,
                           0,
                           0,
                           0);
    if (_xtoq_request_check(conn_two, cookie, "Failed to put image into pixmap")) {
        exit(1);
    }
    
    /* Copy the pixmap into the new window */
    cookie = xcb_copy_area(conn_two,
                           pixmap,
                           window,
                           gc,
                           0,
                           0,
                           0,
                           0,
                           geom_reply->width / 2,
                           geom_reply->height / 2);
    if (_xtoq_request_check(conn_two, cookie, "Failed to put image into pixmap")) {
        exit(1);
    }
    
    xcb_flush(conn_two);
    _xtoq_write_window_info(conn_two, window);
    
    //Remove later
    /* Enter infinte loop so the window stays open */
  //  while (1) {
  //  }
    
    
    /* Never get here, but if we could, would still want to clean up memory */
    free(geom_reply);
    
    // Move disconnects into destructor function
    xcb_disconnect(context.conn);
    xcb_disconnect(conn_two);
    
    return 0;
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

xtoq_event_t
xtoq_wait_for_event (xtoq_context_t context)
{   
    xcb_generic_event_t *evt;
    xtoq_event_t return_evt;
    
    evt = xcb_wait_for_event(context.conn);
    if ((evt->response_type & ~0x80) == _damage_event) {
        return_evt.event_type = XTOQ_DAMAGE;
        return_evt.context = NULL;
        
    } else {
        switch (evt->response_type & ~0x80) {
            case XCB_EXPOSE: {
                xcb_expose_event_t *exevnt = (xcb_expose_event_t *)evt;
                
                printf("Window %u exposed. Region to be redrawn at location (%d, %d), ",
                       exevnt->window, exevnt->x, exevnt->y);
                printf("with dimentions (%d, %d).\n", exevnt->width, exevnt->height);
                
                return_evt.event_type = XTOQ_EXPOSE;
                free(exevnt);
                break;
            }
            case XCB_CREATE_NOTIFY: {
                // Window created as child of root window
                xcb_create_notify_event_t *notify = (xcb_create_notify_event_t *)evt;
                
                return_evt.event_type = XTOQ_CREATE;
                // Create the memory for a new context - this will need to be freed when
                // the window is destroyed
                return_evt.context = malloc(sizeof(xtoq_context_t));
                return_evt.context->conn = context.conn;
                return_evt.context->window = notify->window;
                return_evt.context->parent = notify->parent;
                return_evt.context->x = notify->x;
                return_evt.context->y = notify->y;
                return_evt.context->window = notify->width;
                return_evt.context->window = notify->height;

                free(notify);
                
                // TODO: Add the context created here to the data structure
                
                break;
            }
            case XCB_DESTROY_NOTIFY: {
                // Window destroyed in root window
                xcb_destroy_notify_event_t *notify = (xcb_destroy_notify_event_t *)evt;
                return_evt.event_type = XTOQ_DESTROY;
                
                // Memory for context will need to be freed by caller
                // Only setting the window - other values will be garbage.
                return_evt.context = malloc(sizeof(xtoq_context_t));
                return_evt.context->conn = context.conn;
                return_evt.context->window = notify->window;
                
                free(notify);
                
                // TODO: Remove the window from the data structure.
                // Need to figure out where memory is freed.
                
                break;
            }
            default: {
                printf("UNKNOWN EVENT\n");
                break;
            }
        }
    }
    return return_evt;
}


// key code corresponds roughly to a physical key
// while a keysym corresponds to the symbol on the key top
// http://cgit.freedesktop.org/xcb/demo/tree/app/xte/xte.c
uint8_t *
thing_to_keycode( xcb_connection_t *c, char *thing ) {
    
    xcb_keycode_t *kc;
    xcb_keysym_t ks;
    
    /* For now, assume thing[0] == Latin-1 keysym */
    ks = (uint8_t)thing[0];
    
    kc = xcb_key_symbols_get_keycode( syms, ks );
    
    printf( "String '%s' maps to keysym '%d'\n", thing, ks );
    printf( "String '%s' maps to keycode '%s' - something wrong here\n", thing, kc ); // keycode not correct
    
    return( kc );
}


void
xtoq_key_press (xtoq_context_t context, int window, unsigned short keyCode, unsigned short aChar, char * charAsCharStar)
{
    // move to setup
        syms = xcb_key_symbols_alloc(context.conn );
    //
    xcb_window_t none = { XCB_NONE };
    static xcb_keysym_t shift = { XK_Shift_L };
    
    uint8_t * code;
    uint8_t * wrap_code = NULL;
    
    const char *cap = "~!@#$%^&*()_+{}|:\"<>?";
    if (charAsCharStar[0] >= 'A' && charAsCharStar[0] <= 'Z')
        wrap_code = xcb_key_symbols_get_keycode( syms, shift );
    else if (strchr(cap, charAsCharStar[0]) != NULL)
        wrap_code = xcb_key_symbols_get_keycode( syms, shift );
    
    code = thing_to_keycode( context.conn, charAsCharStar );
  
    if( wrap_code ){
        xcb_test_fake_input( context.conn, XCB_KEY_PRESS, *wrap_code, 0, none, 0, 0, 0 );  
        printf("wrapcode\n");
    }

    else{
        xcb_test_fake_input( context.conn, XCB_KEY_PRESS, *code, 0, context.parent, 0, 0, 0 );  
        xcb_test_fake_input( context.conn, XCB_KEY_RELEASE, *code, 0, context.parent, 0, 0, 0 );
        //xcb_test_fake_input( context.conn, XCB_KEY_PRESS, *charAsCharStar, 0, context.parent, 0, 0, 0 );  // have to look at xcb_keysyms
        //xcb_test_fake_input( context.conn, XCB_KEY_RELEASE, *charAsCharStar, 0, context.parent, 0, 0, 0 );
    }
    printf("key press received by xtoq.c - xcb keycode '%s' from Mac keyCode '%i' in Mac window #%i - (ASCII %hu)\n"
           , code, keyCode, window, aChar);
}

void
xtoq_button_down (xtoq_context_t context, long x, long y, int window)
{
    xcb_window_t none = { XCB_NONE };
    xcb_test_fake_input (context.conn,
                         XCB_BUTTON_PRESS,
                         43,
                         0,
                         none,//context.parent, // remove context to see "UNKNOWN EVENT" message
                         x, // has to be translated (?in the view)
                         y,
                         0 );
    printf("button down received by xtoq.c - (%ld,%ld) in Mac window #%i\n", x, y, window);
}

/* #endif //_XTOQ_C_ */
