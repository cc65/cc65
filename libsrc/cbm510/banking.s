;
; Ullrich von Bassewitz, 13.09.2001
;
; Banking routines for the 510.
;

 	.export		set_bank, sys_bank, restore_bank
	.importzp     	ptr1

	.include      	"cbm510.inc"

.code

.proc	sys_bank
	pha
	lda  	IndReg
	sta  	IndSegSave
	lda  	#$0F
	sta  	IndReg
	pla
	rts
.endproc

.proc	set_bank
	pha
	lda  	IndReg
	sta  	IndSegSave
	pla
	sta  	IndReg
	rts
.endproc

.proc	restore_bank
	pha
	lda	IndSegSave
	sta	IndReg
	pla
	rts
.endproc


