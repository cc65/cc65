;
; zeropage.inc
;
; (C) Copyright 2002-2012, Ullrich von Bassewitz (uz@cc65.org)
;

; Assembler include file that imports the runtime zero page locations used
; by the compiler, ready for usage in asm code.


        .globalzp       sp, sreg, regsave
        .globalzp       ptr1, ptr2, ptr3, ptr4
        .globalzp       tmp1, tmp2, tmp3, tmp4
        .globalzp       regbank
  
; The size of the register bank
regbanksize     = 6

; The total amount of zero page space used
zpspace         = 26

; The amount of space that needs to be saved by an interrupt handler that
; calls C code (does not include the register bank, which is saved by the
; generated C code if required).
zpsavespace     = zpspace - regbanksize

