#
# Watcom Makefile for the cc65 binutils
#

SUBDIRS =	  	\
	common	  	\
	ar65	  	\
	ca65	  	\
	cc65	  	\
	cl65	  	\
        co65            \
        da65            \
	grc	  	\
	ld65		\
	od65

all:	win32


win32:
     	for i in $(SUBDIRS); do $(MAKE) TARGET=NT -C $${i} -f make/watcom.mak; done


dos32:
	for i in $(SUBDIRS); do $(MAKE) TARGET=DOS32 -C $${i} -f make/watcom.mak; done

os2:
	for i in $(SUBDIRS); do $(MAKE) TARGET=OS2 -C $${i} -f make/watcom.mak; done

clean:
	for i in $(SUBDIRS); do $(MAKE) -C $${i} -f make/watcom.mak clean; done

zap:
	for i in $(SUBDIRS); do $(MAKE) -C $${i} -f make/watcom.mak zap; done

strip:
	for i in $(SUBDIRS); do $(MAKE) -C $${i} -f make/watcom.mak strip; done



