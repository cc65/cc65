;
; Ullrich von Bassewitz, 28.09.1998
;
; Banking routines for the 610.
;

 	.export		set_bank, sys_bank, restore_bank
	.importzp     	ptr1

	.include      	"zeropage.inc"

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


