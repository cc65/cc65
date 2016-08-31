
all: chklegal.bin chkillegal.bin chkall.bin chk4510.bin
	@#

.PHONY: chklegal.bin chkillegal.bin chkall.bin chk4510.bin

chk4510.bin: 4510all.s
	$(MAKE) -C ../../src all
	../../bin/cl65 --target none --cpu 4510 --listing 4510all.lst -o $@ $<
	diff -q 4510all.ref $@ || cat 4510all.lst

chklegal.bin: legal.s
	../../bin/cl65 --target none --cpu 6502X -o chklegal.bin legal.s
	diff -q legal.ref chklegal.bin || hex chklegal.bin

chkillegal.bin: illegal.s
	../../bin/cl65 --target none --cpu 6502X -o chkillegal.bin illegal.s
	diff -q illegal.ref chkillegal.bin || hex chkillegal.bin

chkall.bin: all.s
	../../bin/cl65 --target none --cpu 6502X -o chkall.bin all.s

ref: legal.s illegal.s
	../../bin/cl65 --target none --cpu 6502X -o legal.ref legal.s
	../../bin/cl65 --target none --cpu 6502X -o illegal.ref illegal.s

clean:
	rm -f legal.o chklegal.bin
	rm -f illegal.o chkillegal.bin
	rm -f all.o chkall.bin
	rm -f 4510all.o chk4510.bin 4510all.lst

