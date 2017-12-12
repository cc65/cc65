all: booth.o boothtest.o boothtest

boothtest.o: boothtest.c booth.h
	cl65 -t sim6502 -c boothtest.c
		
booth.o: booth.s
	ca65 booth.s
	
boothtest: booth.o boothtest.o
	cl65 -t sim6502 boothtest.o booth.o

