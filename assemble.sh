#!/bin/bash
#
# Testbin.bin is the original file off the EPROM
# Testbin.asm is the MODIFIED file from da65
# Testbin_non_modified.asm is the file straight out of the da65 assembler
# Testbin_reassembled is the file from the ca65 assembler / linker
# 
# other files:
# m740.cfg in the directory cfg is the linker file
# Testbin.in is the input file for the disassembler
#
#
#
#

cd ..
#cc65/bin/ca65 --cpu m740 -o Testbin_reassembled.o -l Testbin.lst Testbin.asm

#cc65/bin/ca65 --cpu m740 -o Testbin_reassembled.o Testbin_non_modified.asm
cc65/bin/ca65 --cpu m740 -o Testbin_reassembled.o Testbin.asm
cc65/bin/ld65 -C m740.cfg -o Testbin_reassembled.bin Testbin_reassembled.o
rm Testbin_reassembled.o
#Compare
xxd Testbin.bin > Testbin.hex
xxd Testbin_reassembled.bin > Testbin_reassembled.hex
diff Testbin.hex Testbin_reassembled.hex


cd cc65