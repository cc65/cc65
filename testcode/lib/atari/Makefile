
all: charmapping.xex defdev.xex displaylist.xex mem.xex multi.xex ostype.xex \
	scrcode.com sys.xex

charmapping.xex: charmapping.c
	cl65 -t atari -o charmapping.xex charmapping.c
defdev.xex: defdev.c
	cl65 -t atari -o defdev.xex defdev.c
displaylist.xex: displaylist.c
	cl65 -t atari -o displaylist.xex displaylist.c
mem.xex: mem.c ../getsp.s
	cl65 -t atari -o mem.xex mem.c ../getsp.s
multi.xex: multi-xex.s multi-xex.cfg
	cl65 -t atari -C multi-xex.cfg multi-xex.s -o multi.xex
ostype.xex: ostype.c
	cl65 -t atari -o ostype.xex ostype.c
scrcode.com: scrcode.s
	ca65 -t atari -o scrcode.o scrcode.s
	ld65 -C atari-asm.cfg -o scrcode.com scrcode.o
sys.xex: sys.c
	cl65 -t atari -o sys.xex sys.c

clean:
	$(RM) charmapping.xex
	$(RM) defdev.xex
	$(RM) displaylist.xex
	$(RM) mem.xex
	$(RM) multi.xex
	$(RM) ostype.xex
	$(RM) scrcode.o
	$(RM) scrcode.com
	$(RM) sys.xex
