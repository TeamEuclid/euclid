# Copyright (c) 2011 Jess VanDerwalker
#

CC = gcc
CFLAGS = -Wall -g -c
LDFLAGS = `pkg-config --libs xcb xcb-aux xcb-damage xcb-composite xcb-image`

# Sources
SOURCES    = lpxcb.c  lpxcb_table.c lpxcb_damage.c lpxcb_event.c lpxcb_util.c lpxcb_api.c lpxcb_data.h
OBJECTS    = $(SOURCES:.c=.o)

# Name of executable target:
EXECUTABLE = lpxcb-ex

# XCB specific flags:
CFLAGS	+= `pkg-config --libs xcb xcb-aux xcb-damage xcb-composite xcb-image`


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
