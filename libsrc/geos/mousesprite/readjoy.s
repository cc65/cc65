;
; Ullrich von Bassewitz, 23.09.1998
;
; unsigned readjoy (unsigned char joy);
;

     	.export		_readjoy

	.include	"../inc/geossym.inc"

.proc	_readjoy

	php
     	sei			; disable IRQ
	lda	$01
	pha
	lda	#$35
	sta	$01		; enable I/O

	tax			; Joystick number into X
	bne    	joy2

; Read joystick 1

joy1:	
	lda	#$7F
     	sta	cia1base
     	lda	cia1base+1
back:	tax
	pla
	sta	$01
	plp
	txa
     	and	#$1F
     	eor	#$1F
	ldx	#0
     	rts

; Read joystick 2

joy2:	lda	#$E0
	ldy	#$FF
	sta	cia1base+2
	lda	cia1base+1
	sty	cia1base+2
	jmp	back

.endproc
