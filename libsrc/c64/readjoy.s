;
; Ullrich von Bassewitz, 23.09.1998
;
; unsigned readjoy (unsigned char joy);
;

     	.export		_readjoy

	.include	"c64.inc"


.proc	_readjoy

	tax			; Joystick number into X
	bne    	joy2

; Read joystick 1

joy1:	lda	#$7F
     	sei
     	sta	CIA1_PRA
     	lda	CIA1_PRB
     	cli
     	and	#$1F
     	eor	#$1F
     	rts

; Read joystick 2

joy2:	ldx	#0
	lda	#$E0
	ldy	#$FF
	sta	CIA1_DDRA
	lda	CIA1_PRA
	sty	CIA1_DDRA
	and	#$1F
	eor	#$1F
	rts

.endproc

