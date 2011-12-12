# Copyright (c) 2011 Jess VanDerwalker
#

CC = gcc
CFLAGS = -Wall -g -c
LDFLAGS = `pkg-config --libs xcb xcb-aux`

# Sources
SOURCES    = lpxcb.c lpxcb_api.c
OBJECTS    = $(SOURCES:.c=.o)

# Name of executable target:
EXECUTABLE = lpxcb-ex

# XCB specific flags:
CFLAGS	+= `pkg-config --libs xcb xcb-aux`


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
