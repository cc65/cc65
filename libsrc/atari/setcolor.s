;
; Christian Groessler, June 2000
;
; this file provides a setcolor() function similar to BASIC SETCOLOR
;
; void __fastcall__ setcolor     (unsigned char color_reg, unsigned char hue, unsigned char luminace);
; void __fastcall__ setcolor_low (unsigned char color_reg, unsigned char color_value);
; unsigned char __fastcall__ getcolor (unsigned char color_reg);
;


 	.export		_setcolor, _setcolor_low, _getcolor
	.import		popa
	.include	"atari.inc"

.proc	_getcolor

	tax
	lda	COLOR0,x	; get current value
	ldx	#0
	rts

.endproc

.proc	_setcolor

;	asl	a		; not -> BASIC compatible
	sta	lum		; remember luminance
	jsr	popa		; get hue
	asl	a
	asl	a
	asl	a
	asl	a
	ora	lum
;	jmp	_setcolor_low

.endproc

.proc	_setcolor_low

	pha
	jsr	popa
	tax
	pla
	sta	COLOR0,x
	rts

.endproc

	.bss
lum:	.res	1
