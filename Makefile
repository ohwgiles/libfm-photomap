CC ?= gcc
CFLAGS ?= -Wall
LDFLAGS ?= 
INSTALL ?= install
DESTDIR ?= /usr
LIBDIR ?= /lib

DEPS = libfm libexif osmgpsmap-1.0
SRC = libfm-photomap.c
SOFILE = libfm-photomap.so

$(SOFILE): $(SRC) Makefile
	$(CC) -o $@ $(CFLAGS) -shared -fPIC $< $(shell pkg-config --cflags --libs $(DEPS))

install: $(SOFILE)
	$(INSTALL) -m 755 $(SOFILE) $(DESTDIR)$(LIBDIR)/libfm/modules/$(SOFILE)
