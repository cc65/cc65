;
; Ullrich von Bassewitz, 17.06.1998
;
; int _fdesc (void);
; /* Find a free descriptor slot */


       	.export		__fdesc
	.import		return0, __filetab
	.importzp	tmp1

__fdesc:
	ldy	#0
L1:	lda	__filetab+1,y	; load flags
	beq    	L2		; jump if empty (== CLOSED)
	iny
	iny
	cpy	#16		; Done?
	bne	L1

; File table is full

	jmp	return0

; Free slot found

L2:    	sty	tmp1		; Offset
	lda	#<__filetab
	ldx	#>__filetab
	clc
	adc	tmp1
	tay
	txa
	adc	#0
	tax
	tya
	rts


