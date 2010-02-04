;
; Ullrich von Bassewitz, 2010-11-03
;
; CC65 runtime: 16x16 => 32 unsigned multiplication
;

        .export         umul16x16r32, umul16x16r32m

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 16x16 => 32 unsigned multiplication routine.
;
;  routine         lhs         rhs        result          result also in
; -----------------------------------------------------------------------
;  umul16x16r32    ptr1        ax         ax:sreg          ptr1:sreg
;  umul16x16r32m   ptr1        ptr3       ax:sreg          ptr1:sreg
;
; ptr3 is left intact by the routine.
;

umul16x16r32:
        sta     ptr3
        stx     ptr3+1

umul16x16r32m:
       	lda	#0
       	sta    	sreg+1
       	ldy    	#16   	       	; Number of bits

        lsr     ptr1+1
        ror     ptr1            ; Get first bit into carry
@L0:    bcc     @L1

      	clc
      	adc	ptr3
      	pha
       	lda     ptr3+1
      	adc	sreg+1
      	sta	sreg+1
      	pla

@L1:    ror     sreg+1
     	ror	a
     	ror	ptr1+1
     	ror	ptr1
        dey
        bne     @L0

        sta     sreg            ; Save byte 3
      	lda	ptr1	       	; Load the result
      	ldx	ptr1+1
      	rts	    		; Done

