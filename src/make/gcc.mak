#
# gcc Makefile for the program sources
#

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

.PHONY: all dist clean zap
all dist clean zap:
	for i in $(SUBDIRS); do $(MAKE) -C $$i -f make/gcc.mak $@ || exit $$?; done

