lib_LTLIBRARIES = libcompositewm.la
AM_CFLAGS = $(XCB_CFLAGS) $(BASE_CFLAGS)

INCLUDES = -I${top_srcdir}/include

libcompositewm_la_LDFLAGS = -version-number 0:0:0

libcompositewm_la_LIBADD = $(XCB_LIBS)

libcompositewm_la_SOURCES = \
	window.c \
	context_list.c \
	event_loop.c \
	init.c \
	util.c \
	xtoq.c
