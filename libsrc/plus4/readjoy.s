;
; Ullrich von Bassewitz, 23.09.1998
;
; unsigned readjoy (unsigned char joy);
;

     	.export		_readjoy

	.include	"plus4.inc"


.proc	_readjoy

	ldy	#$FA		; Load index for joystick #1
	tax			; Test joystick number
	beq    	L1
	ldy	#$FB		; Load index for joystick #2
L1:    	sei
	sty	TED_KBD
	lda	TED_KBD
	cli
	ldx	#$00		; Clear high byte
     	and	#$1F
     	eor	#$1F
     	rts

.endproc


