;
; Stefan Haubenthal, 2005-06-08
;
; CBM device to string
;

	.export		devicestr
	.import		pusha0, tosudiva0
	.importzp	sreg, ptr1, ptr2

	.macpack	generic

;------------------------------------------------------------------------------
; Convert unit number in A into string representation pointed to by ptr2.

.proc	devicestr

	jsr	pusha0
	lda	#10
	jsr	tosudiva0
	ldy	#0
	lda	sreg
	beq	:+		; >=10
	add	#'0'
	sta	(ptr2),y
	iny
:	lda	ptr1		; rem
	add	#'0'
	sta	(ptr2),y
	iny
	lda	#0
	sta	(ptr2),y
	rts

.endproc
