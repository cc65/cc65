.PHONY: all clean test

# Size of cartridge to generate.
# Possible values:
#  8K = 0x2000
# 16K = 0x4000
# 32K = 0x8000
CARTSIZE := 0x2000

ifeq (${CARTSIZE},0x8000)
COUNT := 3
else
COUNT := 1
endif

all: conio.pce

%.pce: %.bin
	dd if=$< bs=8K skip=${COUNT} > $@
	dd if=$< bs=8K count=${COUNT} >> $@

%.bin: %.c ../../../lib/pce.lib
	../../../bin/cl65 -t pce $< -Wl -D__CARTSIZE__=${CARTSIZE} -m $*.map -o $@

clean:
	$(RM) conio.o conio.???

test: conio.pce
	mednafen -force_module pce $<
