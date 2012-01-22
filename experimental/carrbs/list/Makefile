# Copyright (c) 2012 Benjamin Carr
#
# Makefile for the context_list project
#

CC = gcc
CFLAGS = -I/opt/X11/include/ -Wall -g -c
LDFLAGS = `pkg-config --libs xcb xcb-aux xcb-damage xcb-composite xcb-image`

# Sources
SOURCES    = context_list.h context_list.c
OBJECTS    = $(SOURCES:.c=.o)

# Name of executable target:
EXECUTABLE = context_list

# XCB specific flags:
CFLAGS	+= `pkg-config --libs xcb xcb-aux xcb-damage xcb-composite xcb-image`


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
