;
; Ullrich von Bassewitz, 26.05.2002
;
; CC65 runtime: zeropage usage
;

	.exportzp   	sp, sreg, regsave
	.exportzp   	ptr1, ptr2, ptr3, ptr4
	.exportzp   	tmp1, tmp2, tmp3, tmp4
	.exportzp   	regbank

; ------------------------------------------------------------------------

.zeropage

sp:	      	.res   	2      	; Stack pointer
sreg:	      	.res	2      	; Secondary register/high 16 bit for longs
regsave:      	.res	4      	; slot to save/restore (E)AX into
ptr1:	      	.res	2
ptr2:	      	.res	2
ptr3:	      	.res	2
ptr4:	      	.res	2
tmp1:	      	.res	1
tmp2:	      	.res	1
tmp3:	      	.res	1
tmp4:	      	.res	1
regbank:      	.res	6      	; 6 byte register bank

; Add an empty EXTZP zeropage segment to avoid linker warnings that this
; segment does not exist (it does not exist in most builtin linker configs
; but is used when linking modules).

.segment        "EXTZP" : zeropage

