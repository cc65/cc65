;
; Ullrich von Bassewitz, 18.08.1998
;
; Stack checker
;


	.export		_stkcheck, __stksafety
	.import		pushax, exit
	.import		__hend
	.importzp	sp

.data
__stksafety:
     	.word	64			;

.code
_stkcheck:
	clc
	lda	__hend
	adc	__stksafety
	tax				; Remember low byte
	lda	__hend+1
	adc	__stksafety+1

	cmp	sp+1
	bcc	Ok
       	bne	L1
	cpx	sp
	bcc	Ok

; Stack overflow

L1:    	inc	sp+1			; Create 256 bytes of space
	ldx    	#0
	lda	#4
	jsr	pushax
	jmp	exit

; All is well

Ok:	rts






