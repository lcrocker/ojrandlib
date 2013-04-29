# OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
#
# Linux Makefile -- requires Gmake 3.8, realpath, and probably some
# other things I take for granted.
#

ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRCDIR = $(ROOT)/source
BLDDIR = $(ROOT)/build
TESTDIR = $(ROOT)/tests

CC = gcc
CFLAGS = -g -DDEBUG -Wall -fpic
# CFLAGS = -O3 -DNDEBUG -Wall -fpic
JAVACFLAGS = -g -Werror
# JAVACFLAGS = -g:none

LIBNAME = libojrand.so
LIBSRCNAMES = libmain.c entropy.c jkiss.c mt19937.c
LIBSOURCES = $(patsubst %.c,$(SRCDIR)/c/library/%.c,$(LIBSRCNAMES))
LIBOBJECTS = $(patsubst %.c,$(BLDDIR)/%.o,$(LIBSRCNAMES))

TESTSRCNAMES = hello.c
TESTEXES = $(patsubst %.c,$(BLDDIR)/%,$(TESTSRCNAMES))

.PHONY: all lib test clean

all: lib test

lib: $(BLDDIR)/$(LIBNAME)

test: $(TESTEXES)
	cd $(BLDDIR) && ./hello

clean:
	rm -rf $(BLDDIR)/*

$(BLDDIR):
	mkdir -p $(BLDDIR)

$(BLDDIR)/%.o: $(SRCDIR)/c/library/%.c $(SRCDIR)/c/include/ojrandlib.h | $(BLDDIR)
	$(CC) $(CFLAGS) -c -I$(SRCDIR)/c/include -o $@ $<

$(BLDDIR)/$(LIBNAME): $(LIBOBJECTS)
	$(CC) $(CFLAGS) -shared -o $@ $^ -lm

$(BLDDIR)/hello: $(TESTDIR)/c/hello.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/c/include -o $@ $< -lm -lojrand

