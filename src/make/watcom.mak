#
# Watcom Makefile for the cc65 binutils
#

SUBDIRS =		\
	common		\
	ar65		\
	ca65		\
	ld65

all:
	cd common
	make -f make\watcom.mak
	cd ..\ar65
	make -f make\watcom.mak
	cd ..\ca65
	make -f make\watcom.mak
	cd ..\ld65
	make -f make\watcom.mak
	cd ..

clean:
	cd common
	make -f make\watcom.mak clean
	cd ..\ar65
	make -f make\watcom.mak clean
	cd ..\ca65
	make -f make\watcom.mak clean
	cd ..\ld65
	make -f make\watcom.mak clean
	cd ..

strip:
	@cd ar65
	@-make -f make\watcom.mak strip
	@cd ..\ca65
	@-make -f make\watcom.mak strip
	@cd ..\ld65
	@-make -f make\watcom.mak strip
	@cd ..


