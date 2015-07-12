
all: conio.pce

conio.pce: conio.c
	../../../bin/cl65 -t pce conio.c ../../../joy/pce-stdjoy.joy --mapfile conio.map -o conio.pce

clean:
	$(RM) conio.pce

test: conio.pce
	mednafen -force_module pce conio.pce

