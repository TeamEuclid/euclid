# Copyright (c) 2012 Jess VanDerwalker
#
# Makefile for the rootimg project
#

CC = gcc
CFLAGS = -Wall -g -c
LDFLAGS = `pkg-config --libs xcb xcb-aux xcb-damage xcb-composite xcb-image`

# Sources
SOURCES    = rootimg.c
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
