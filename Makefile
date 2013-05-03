# OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
#
# Linux Makefile -- requires Gmake 3.8, realpath, and probably some
# other things I take for granted.
#

ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRCDIR = $(ROOT)/source
TESTDIR = $(ROOT)/tests
BLDDIR = $(ROOT)/build
CLASSDIR = com/onejoker/randlib

CC = gcc
CFLAGS = -g -DDEBUG -Wall -Werror -fpic
# CFLAGS = -O3 -DNDEBUG -Wall -Werror -fpic
CXX = g++
CXXFLAGS = $(CFLAGS)
LD = g++
LDFLAGS =
JAVACFLAGS = -g -Werror
# JAVACFLAGS = -g:none
JPACKAGE = $(subst /,.,$(CLASSDIR))

LIBNAME = libojrand.so
LIBCNAMES = libmain.c capi.c entropy.c zignor.c jkiss.c mt19937.c
LIBOBJECTS = $(patsubst %.c,$(BLDDIR)/%.o,$(LIBCNAMES))
LIBOBJECTS += $(BLDDIR)/wrapper.o
TESTNAMES = hello cpphello hello.py stats
TESTPROGS = $(patsubst %,$(BLDDIR)/%,$(TESTNAMES))

.PHONY: all lib test clean python java

# all: lib python java test
all: lib python java

lib: $(BLDDIR)/$(LIBNAME)

python: $(BLDDIR)/ojrandlib.py

java: $(BLDDIR)/$(CLASSDIR)/Generator.class $(BLDDIR)/com_onejoker_randlib_Generator.h

test: $(TESTPROGS)
	cd $(BLDDIR) && ./hello
	cd $(BLDDIR) && ./cpphello
	cd $(BLDDIR) && ./hello.py
	#cd $(BLDDIR) && ./stats

clean:
	rm -rf $(BLDDIR)/*

$(BLDDIR):
	mkdir -p $(BLDDIR)

$(BLDDIR)/$(CLASSDIR):
	mkdir -p $(BLDDIR)/$(CLASSDIR)

$(BLDDIR)/wrapper.o: $(SRCDIR)/library/wrapper.cc $(SRCDIR)/library/ojrandlib.h | $(BLDDIR)
	$(CXX) $(CXXFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/%.o: $(SRCDIR)/library/%.c $(SRCDIR)/library/ojrandlib.h | $(BLDDIR)
	$(CC) $(CFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/$(LIBNAME): $(LIBOBJECTS)
	$(LD) $(LDFLAGS) -shared -o $@ $^ -lm

$(BLDDIR)/hello: $(TESTDIR)/c/hello.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lm -lojrand

$(BLDDIR)/stats: $(TESTDIR)/c/stats.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lm -lojrand

$(BLDDIR)/cpphello: $(TESTDIR)/cpp/hello.cc $(BLDDIR)/$(LIBNAME)
	$(CXX) $(CXXFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lm -lojrand

$(BLDDIR)/ojrandlib.py: $(SRCDIR)/python/ojrandlib.py
	cp $< $@

$(BLDDIR)/hello.py: $(TESTDIR)/python/hello.py python
	cp $< $@

$(BLDDIR)/$(CLASSDIR)/Generator.class: $(SRCDIR)/java/$(CLASSDIR)/Generator.java
	javac $(JAVACFLAGS) -d $(BLDDIR) $<

$(BLDDIR)/com_onejoker_randlib_Generator.h: $(BLDDIR)/$(CLASSDIR)/Generator.class
	cd $(BLDDIR) && javah -jni $(JPACKAGE).Generator
