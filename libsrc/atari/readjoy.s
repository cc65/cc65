;
; Christian Groessler
;
; unsigned readjoy (unsigned char joy);
;

     	.export		_readjoy

	.include	"atari.inc"


.proc	_readjoy

	and	#3		; fix joystick number
	tax			; Joystick number (0-3) into X

; Read joystick

	lda	STRIG0,x	; get button
	asl	a
	asl	a
	asl	a
	asl	a
	ora	STICK0,x	; add position information
	eor	#$1F
	ldx	#0		; fix X
     	rts

.endproc

