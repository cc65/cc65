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

all:	nt


nt:
	for %%i in ($(SUBDIRS)) do $(MAKE) TARGET=NT -C %%i -f make\watcom.mak


dos:
	for %%i in ($(SUBDIRS)) do $(MAKE) TARGET=DOS32 -C %%i -f make\watcom.mak

os2:
	for %%i in ($(SUBDIRS)) do $(MAKE) TARGET=OS2 -C %%i -f make\watcom.mak

clean:
	for %%i in ($(SUBDIRS)) do $(MAKE) -C %%i -f make\watcom.mak clean

strip:
	for %%i in ($(SUBDIRS)) do $(MAKE) -C %%i -f make\watcom.mak strip



