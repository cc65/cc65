;
; Ullrich von Bassewitz, 23.09.1998
;
; unsigned readjoy (unsigned char joy);
;

     	.export		_readjoy

	.include	"../inc/geossym.inc"

.proc	_readjoy

	tax			; Joystick number into X
	bne    	joy2

; Read joystick 1

joy1:	lda	#$7F
     	sei
     	sta	cia1base
     	lda	cia1base+1
     	cli
     	and	#$1F
     	eor	#$1F
     	rts

; Read joystick 2

joy2:	ldx	#0
	lda	#$E0
	ldy	#$FF
	sta	cia1base+2
	lda	cia1base+1
	sty	cia1base+2
	and	#$1F
	eor	#$1F
	rts

.endproc

