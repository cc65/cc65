
all: conio.pce

conio.pce: conio.c
	../../../bin/cl65 -t pce conio.c --mapfile conio.map -o conio.pce

clean:
	$(RM) conio.pce
	$(RM) conio.map

test: conio.pce
	mednafen -force_module pce conio.pce

