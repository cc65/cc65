#
# gcc Makefile for the program sources
#

CFLAGS 	= -g -O2 -Wall
CC	= gcc
LDFLAGS	=

SUBDIRS	=		\
	common		\
	ar65		\
	ca65		\
	cc65		\
	cl65		\
        co65            \
	da65		\
	grc		\
	ld65		\
	od65

.PHONY: all
all:
	for i in $(SUBDIRS); do $(MAKE) -C $$i -f make/gcc.mak all || exit $$?; done

.PHONY: dist
dist:
	for i in $(SUBDIRS); do $(MAKE) -C $$i -f make/gcc.mak dist || exit $$?; done

.PHONY: clean
clean:
	for i in $(SUBDIRS); do $(MAKE) -C $$i -f make/gcc.mak clean || exit $$?; done

.PHONY: zap
zap:
	for i in $(SUBDIRS); do $(MAKE) -C $$i -f make/gcc.mak zap || exit $$?; done

