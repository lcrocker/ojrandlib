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
CXX = g++
CXXFLAGS = $(CFLAGS)
LD = g++
LDFLAGS =
JAVACFLAGS = -g -Werror
# JAVACFLAGS = -g:none

LIBNAME = libojrand.so
LIBCNAMES = libmain.c entropy.c jkiss.c mt19937.c
LIBOBJECTS = $(patsubst %.c,$(BLDDIR)/%.o,$(LIBCNAMES))
LIBOBJECTS += $(BLDDIR)/wrapper.o

.PHONY: all lib test clean

all: lib test

lib: $(BLDDIR)/$(LIBNAME)

test: $(BLDDIR)/hello $(BLDDIR)/cpphello
	cd $(BLDDIR) && ./hello
	cd $(BLDDIR) && ./cpphello

clean:
	rm -rf $(BLDDIR)/*

$(BLDDIR):
	mkdir -p $(BLDDIR)

$(BLDDIR)/wrapper.o: $(SRCDIR)/library/wrapper.cc $(SRCDIR)/library/ojrandlib.h | $(BLDDIR)
	$(CXX) $(CXXFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/%.o: $(SRCDIR)/library/%.c $(SRCDIR)/library/ojrandlib.h | $(BLDDIR)
	$(CC) $(CFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/$(LIBNAME): $(LIBOBJECTS)
	$(LD) $(LDFLAGS) -shared -o $@ $^ -lm

$(BLDDIR)/hello: $(TESTDIR)/c/hello.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lm -lojrand

$(BLDDIR)/cpphello: $(TESTDIR)/cpp/hello.cc $(BLDDIR)/$(LIBNAME)
	$(CXX) $(CXXFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lm -lojrand
