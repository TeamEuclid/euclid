#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include "rootimg_api.h"

int 
main(int argc, char **argv) {
    
    context_t context = init(argv[1]);
    int ret = get_image(context);
    return 0;
}