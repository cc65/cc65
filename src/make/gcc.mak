#
# gcc Makefile for the program sources
#
PROGS =		\
	ar65	\
	ca65	\
	cc65	\
	chrcvt	\
	cl65	\
	co65	\
	da65	\
       	grc65   \
	ld65	\
	od65

SUBDIRS	= common $(PROGS)

.PHONY: all dist clean zap $(SUBDIRS)
all dist clean zap:	$(SUBDIRS)

# Finish building the common library before allowing parallel makes.
$(PROGS):	common

$(SUBDIRS):
	$(MAKE) -C $@ -f make/gcc.mak $(MAKECMDGOALS)

