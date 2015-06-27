#!/usr/bin/make -f

# these can be overridden using make variables. e.g.
#   make CFLAGS=-O2
#
OPTIMIZATIONS ?= -msse -msse2 -mfpmath=sse -ffast-math -fomit-frame-pointer -O3 -fno-finite-math-only
PREFIX ?= /usr/local
CFLAGS ?= $(OPTIMIZATIONS) -Wall
LIBDIR ?= lib

xfade_VERSION?=$(shell git describe --tags HEAD 2>/dev/null | sed 's/-g.*$$//;s/^v//' || echo "LV2")
###############################################################################

LV2DIR ?= $(PREFIX)/$(LIBDIR)/lv2
LOADLIBES=-lm
LV2NAME=xfade
BUNDLE=xfade.lv2

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
  LV2LDFLAGS=-dynamiclib
  LIB_EXT=.dylib
  EXTENDED_RE=-E
else
  LV2LDFLAGS=-Wl,-Bstatic -Wl,-Bdynamic
  LIB_EXT=.so
  EXTENDED_RE=-r
endif

targets=$(LV2NAME)$(LIB_EXT)

###############################################################################
# extract versions
LV2VERSION=$(xfade_VERSION)
include git2lv2.mk

# check for build-dependencies
ifeq ($(shell pkg-config --exists lv2 || echo no), no)
  $(error "LV2 SDK was not found")
endif

override CFLAGS += -fPIC -std=c99
override CFLAGS += `pkg-config --cflags lv2`

# build target definitions
default: all

all: manifest.ttl $(LV2NAME).ttl $(targets)

manifest.ttl: manifest.ttl.in
	sed "s/@LV2NAME@/$(LV2NAME)/;s/@LIB_EXT@/$(LIB_EXT)/" \
	  manifest.ttl.in > manifest.ttl

$(LV2NAME).ttl: $(LV2NAME).ttl.in
	sed "s/@VERSION@/lv2:microVersion $(LV2MIC) ;lv2:minorVersion $(LV2MIN) ;/g" \
		$(LV2NAME).ttl.in > $(LV2NAME).ttl

$(LV2NAME)$(LIB_EXT): $(LV2NAME).c
	$(CC) $(CPPFLAGS) $(CFLAGS) \
	  -o $(LV2NAME)$(LIB_EXT) $(LV2NAME).c \
	  -shared $(LV2LDFLAGS) $(LDFLAGS) $(LOADLIBES)

# install/uninstall/clean target definitions

install: all
	install -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m755 $(LV2NAME)$(LIB_EXT) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	install -m644 manifest.ttl $(LV2NAME).ttl $(DESTDIR)$(LV2DIR)/$(BUNDLE)

uninstall:
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/manifest.ttl
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(LV2NAME).ttl
	rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(LV2NAME)$(LIB_EXT)
	-rmdir $(DESTDIR)$(LV2DIR)/$(BUNDLE)

clean:
	rm -f manifest.ttl $(LV2NAME).ttl $(LV2NAME)$(LIB_EXT)

.PHONY: clean all install uninstall
