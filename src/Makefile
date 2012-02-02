# Copyright (c) 2012 Jess VanDerwalker <washu@sonic.net>
#
# Makefile for the src/ directory

CFLAGS = -Wall -g -O0 -c
INCLUDES = -I/opt/X11/include
# May need to change this
LDFLAGS = -L/opt/X11/lib -lxcb-util -lxcb-damage -lxcb-composite -lxcb-xfixes -lxcb-render -lxcb-shape -lxcb-image -lxcb-shm -lxcb

SOURCES = xtoq.c util.c
OBJECTS = $(SOURCES:.c=.o)

all: $(SOURCES) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f *.o

.PHONY: all clean