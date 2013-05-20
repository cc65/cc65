;
; Randum number generator
;
; Written and donated by Sidney Cadot - sidney@ch.twi.tudelft.nl
;
; May be distributed with the cc65 runtime using the same license.
;
;
; int rand (void);
; void srand (unsigned seed);
;
;  Uses 4-byte state.
;  Multiplier must be 1 (mod 4)
;  Added value must be 1 (mod 2)
;  This guarantees max. period (2**32)
;  Bits 8-22 are returned (positive 2-byte int)
;  where 0 is LSB, 31 is MSB.
;  This is better as lower bits exhibit easily
;  detectable patterns.
;

	.importzp	ptr1
      	.export		_rand, _srand

.data

; The seed. When srand() is not called, the C standard says that that rand()
; should behave as if srand() was called with an argument of 1 before.
rand:  	.word   1

.code

_rand:
;0 arg. by default.
	lda	#0
	tax
_rand2:	
;Get and randomize argument:
;Add $7219.
	clc
	adc	#$19
	tay
	txa
	adc	#$72
	tax
	tya
;XOR by same amount (ends 0 if orig. 0).
	eor	#$19
	tay
	txa
	eor	#$72
	tax
	tya
;Add current value to seed.
	clc
	adc	rand
	tay
	txa
	adc	rand+1
	tax
	tya
;Add $6292.
	clc
	adc	#$92
	tay
	txa
	adc	#$62
	tax
;XOR by $93F3.
	tya
	eor	#$F3
	tay
	txa
	eor	#$93
	tax
	tya
	sta	ptr1
	stx	ptr1+1
	lsr
	lda	ptr1
	ror	ptr1+1
	ror
	ldx	ptr1+1
;Store new seed.
	sta	rand
	stx	rand+1
;Add $9732.
	clc
	adc	#$32
	tay
	txa
	adc	#$97
	tax
	tya
;XOR by $6065.
	eor	#$65
	tay
	txa
	eor	#$60
	tax
	tya
;Rotate seed right 1 bit.
	sta	ptr1
	tay
	stx	ptr1+1
	txa
	asl
	tya
	rol
	rol	ptr1+1
;Swap bytes.
;	stx	ptr1
;	tax
;	lda	ptr1
;Clear sign bit as in orig. routine.
	tay
	;lda	ptr1+1
	txa
	and	#$7F
	tax
	tya
	rts


_srand:	sta	rand+0		; Store the seed
	stx	rand+1
	rts


