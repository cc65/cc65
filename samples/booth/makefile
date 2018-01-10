TARGET=sim6502

all: booth.o boothtest.o boothtest

boothtest.o: boothtest.c booth.h
	cl65 -t $(TARGET) -O -c boothtest.c
		
booth.o: booth.s
	ca65 booth.s
	
boothtest: booth.o boothtest.o
	cl65 -t $(TARGET) -o boothtest boothtest.o booth.o
