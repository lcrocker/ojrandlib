# OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
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
CFLAGS = -g -DDEBUG -Wall -std=c99 -pedantic -fpic
# CFLAGS = -O3 -DNDEBUG -Wall -std=c99 -pedantic -fpic
CXX = g++
CXXFLAGS = -g -DDEBUG -Wall -std=c++98 -pedantic -fpic
LD = g++
SYSTEMLIBS = -lm
JAVA_HOME ?= /usr/java
JAVACFLAGS = -g -Werror
# JAVACFLAGS = -g:none
JPACKAGE = $(subst /,.,$(CLASSDIR))

LIBNAME = libojrand.so
LDFLAGS = -nostartfiles

LIBCNAMES = init.c generator.c capi.c entropy.c ziggurat.c randomorg.c
ALGORITHMS = algorithms.c jkiss127.c mt19937.c mwc8222.c
TESTNAMES = hello cpphello hello.py Hello.class functions

LIBCNAMES += $(ALGORITHMS)
LIBOBJECTS = $(patsubst %.c,$(BLDDIR)/%.o,$(LIBCNAMES))
LIBOBJECTS += $(BLDDIR)/wrapper.o $(BLDDIR)/jniGenerator.o
TESTPROGS = $(patsubst %,$(BLDDIR)/%,$(TESTNAMES))

.PHONY: all lib test clean python java randtest

all: lib python java test

lib: $(BLDDIR)/$(LIBNAME)

python: $(BLDDIR)/ojrandlib.py

java: $(BLDDIR)/$(CLASSDIR)/Generator.class $(BLDDIR)/com_onejoker_randlib_Generator.h

test: $(TESTPROGS)
	cd $(BLDDIR) && ./hello
	cd $(BLDDIR) && ./cpphello
	cd $(BLDDIR) && ./hello.py
	cd $(BLDDIR) && java -ea -cp "." -Djava.library.path="." Hello
	cd $(BLDDIR) && ./functions

randtest: $(BLDDIR)/random
	cd $(BLDDIR) && ./random

clean:
	rm -rf $(BLDDIR)/*

$(BLDDIR):
	mkdir -p $(BLDDIR)

$(BLDDIR)/$(CLASSDIR):
	mkdir -p $(BLDDIR)/$(CLASSDIR)

$(BLDDIR)/wrapper.o: $(SRCDIR)/library/wrapper.cc $(SRCDIR)/library/ojrandlib.h | $(BLDDIR)
	$(CXX) $(CXXFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/jniGenerator.o: $(SRCDIR)/java/$(CLASSDIR)/jniGenerator.c $(SRCDIR)/library/ojrandlib.h | $(BLDDIR)
	$(CC) $(CFLAGS) -c -I$(JAVA_HOME)/include -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/%.o: $(SRCDIR)/library/%.c $(SRCDIR)/library/ojrandlib.h | $(BLDDIR)
	$(CC) $(CFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/$(LIBNAME): $(LIBOBJECTS)
	$(LD) $(LDFLAGS) -shared -o $@ $^ $(SYSTEMLIBS)

$(BLDDIR)/hello: $(TESTDIR)/c/hello.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lojrand

$(BLDDIR)/functions: $(TESTDIR)/c/functions.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lojrand

$(BLDDIR)/random: $(TESTDIR)/c/random.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lm -lojrand

$(BLDDIR)/cpphello: $(TESTDIR)/cpp/hello.cc $(BLDDIR)/$(LIBNAME)
	$(CXX) $(CXXFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lm -lojrand

$(BLDDIR)/ojrandlib.py: $(SRCDIR)/python/ojrandlib.py
	cp $< $@

$(BLDDIR)/hello.py: $(TESTDIR)/python/hello.py python
	cp $< $@

$(BLDDIR)/$(CLASSDIR)/Generator.class: $(SRCDIR)/java/$(CLASSDIR)/Generator.java | $(BLDDIR)/$(CLASSDIR)
	javac $(JAVACFLAGS) -d $(BLDDIR) $<

$(BLDDIR)/com_onejoker_randlib_Generator.h: $(BLDDIR)/$(CLASSDIR)/Generator.class
	cd $(BLDDIR) && javah -jni -force $(JPACKAGE).Generator

$(BLDDIR)/Hello.class: $(TESTDIR)/java/Hello.java | $(BLDDIR)
	javac $(JAVACFLAGS) -cp $(BLDDIR) -d $(BLDDIR) $<
