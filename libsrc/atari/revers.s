;
; Ullrich von Bassewitz, 07.08.1998
;
; unsigned char revers (unsigned char onoff);
;
	.include "atari.inc"
	
      	.export		_revers

_revers:
	ldx    	#$00		; Assume revers off
	tay			; Test onoff
	beq	L1		; Jump if off
	ldx	#$80		; Load on value
L1:	ldy	#$00		; Assume old value is zero
	lda    	INVFLG 	       	; Load old value
	stx	INVFLG		; Set new value
	beq	L2		; Jump if old value zero
	iny			; Make old value = 1
L2:	ldx	#$00		; Load high byte of result
	tya			; Load low byte, set CC
	rts

