
all: chklegal.bin chkillegal.bin
	@#

.PHONY: chklegal.bin chkillegal.bin

chklegal.bin: legal.s
	../../bin/cl65 --target none --cpu 6502X -o chklegal.bin legal.s
	diff legal.ref chklegal.bin

chkillegal.bin: illegal.s
	../../bin/cl65 --target none --cpu 6502X -o chkillegal.bin illegal.s

ref: legal.s
	../../bin/cl65 --target none --cpu 6502X -o legal.ref legal.s

clean:
	rm -f legal.o chklegal.bin
	rm -f illegal.o chkillegal.bin
