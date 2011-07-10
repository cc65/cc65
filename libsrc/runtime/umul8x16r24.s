;
; Ullrich von Bassewitz, 2011-07-10
;
; CC65 runtime: 8x16 => 24 unsigned multiplication
;

        .export         umul8x16r24, umul8x16r24m
        .export         umul8x16r16, umul8x16r16m

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 8x16 => 24 unsigned multiplication routine. Because the overhead for a
; 8x16 => 16 unsigned multiplication routine is small, we will tag it with
; the matching labels as well.
;
;  routine         lhs         rhs        result          result also in
; -----------------------------------------------------------------------
;  umul8x16r24     ptr1-lo     ax         ax:sreg-lo      ptr1:sreg-lo
;  umul8x16r24m    ptr1-lo     ptr3       ax:sreg-lo      ptr1:sreg-lo
;
; ptr3 ist left intact by the routine.
;

umul8x16r24:
umul8x16r16:
        sta     ptr3
        stx     ptr3+1

umul8x16r24m:
umul8x16r16m:
       	ldx   	#0
        stx     ptr1+1
       	stx    	sreg

       	ldy    	#8              ; Number of bits
        ldx     ptr3            ; Get into register for speed
        lda     ptr1
        ror     a               ; Get next bit into carry
@L0:    bcc     @L1

        clc
        pha
        txa
        adc     ptr1+1
        sta     ptr1+1
        lda     ptr3+1
        adc     sreg
        sta     sreg
        pla

@L1:    ror     sreg
     	ror   	ptr1+1
     	ror   	a
        dey
        bne     @L0

        sta     ptr1            ; Save low byte of result
        ldx     ptr1+1          ; Load high byte of result
      	rts   	    		; Done


