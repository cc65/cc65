#
# Watcom Makefile for the cc65 binutils
#

SUBDIRS =	  	\
	common	  	\
	ar65	  	\
	ca65	  	\
	cc65	  	\
	cl65	  	\
	grc	  	\
	ld65		\
	od65

all:	nt


nt:
	cd common
	make -f make\watcom.mak
	cd ..\ar65
	make -f make\watcom.mak
	cd ..\ca65
	make -f make\watcom.mak
	cd ..\cc65
	make -f make\watcom.mak
	cd ..\cl65
	make -f make\watcom.mak
	cd ..\grc
	make -f make\watcom.mak
	cd ..\ld65
	make -f make\watcom.mak
	cd ..\od65
	make -f make\watcom.mak
	cd ..

dos:
   	cd common
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..\ar65
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..\ca65
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..\cc65
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..\cl65
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..\grc
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..\ld65
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..\od65
       	make -DTARGET=DOS32 -f make\watcom.mak
	cd ..

os2:
   	cd common
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..\ar65
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..\ca65
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..\cc65
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..\cl65
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..\grc
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..\ld65
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..\od65
       	make -DTARGET=OS2 -f make\watcom.mak
	cd ..

clean:
	cd common
	make -f make\watcom.mak clean
	cd ..\ar65
	make -f make\watcom.mak clean
	cd ..\ca65
	make -f make\watcom.mak clean
	cd ..\cc65
	make -f make\watcom.mak clean
	cd ..\cl65
	make -f make\watcom.mak clean
	cd ..\grc
	make -f make\watcom.mak clean
	cd ..\ld65
	make -f make\watcom.mak clean
	cd ..\od65
	make -f make\watcom.mak clean
	cd ..

strip:
	@cd ar65
	@-make -f make\watcom.mak strip
	@cd ..\ca65
	@-make -f make\watcom.mak strip
       	@cd ..\cc65
	@-make -f make\watcom.mak strip
	@cd ..\cl65
	@-make -f make\watcom.mak strip
	@cd ..\grc
	@-make -f make\watcom.mak strip
	@cd ..\ld65
	@-make -f make\watcom.mak strip
	@cd ..\od65
	@-make -f make\watcom.mak strip
	@cd ..



