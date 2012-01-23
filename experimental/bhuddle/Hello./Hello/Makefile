# Makefile for /Hello folder


CFLAGS = -g -O0 --verbose
FRAMEWORKS = -framework Cocoa
IBTOOL = ibtool

SOURCES = main.m AppDelegate.m HelloView.m
XIBDIR = en.lproj/
XIBS = $(XIBDIR)MainMenu.xib
NIBS = $(XIBDIR) $(XIBS:.xib=.nib)

all: imageflip

imageflip: $(SOURCES) xibs
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(SOURCES) $(FRAMEWORKS)

xibs: $(XIBS)
	$(IBTOOL) $(XIBS)

clean:
	rm -f imageflip
	rm -rf imageflip.dSYM
	rm -f Info.plist

.PHONY: all clean