# Copyright (c) 2012 David Snyder
#
# Makefile for the rootimg project
#

CC = gcc
CFLAGS = -I/opt/X11/include/ -Wall -g -c
LDFLAGS = `pkg-config --libs xcb xcb-aux xcb-damage xcb-composite xcb-image`

# Sources
SOURCES    = main.c rootimg_api.c util.c
OBJECTS    = $(SOURCES:.c=.o)

# Name of executable target:
EXECUTABLE = rootimg

# XCB specific flags:
CFLAGS	+= `pkg-config --libs xcb xcb-aux xcb-damage xcb-composite xcb-image`


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
