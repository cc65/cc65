;
; zeropage.inc
;
; (C) Copyright 2002 Ullrich von Bassewitz (uz@cc65.org)
;

; Assembler include file that imports the runtime zero page locations used
; by the compiler, ready for usage in asm code.


	.importzp   	sp, sreg, regsave
	.importzp   	ptr1, ptr2, ptr3, ptr4
	.importzp   	tmp1, tmp2, tmp3, tmp4
	.importzp   	regbank

; The total amount of zero page space used

zpspace = 26




