;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: modulo operation for long signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

       	.export		tosmodeax
  	.import		poplsargs, udiv32, negeax
  	.importzp	sreg, ptr1, ptr2, tmp1, tmp3, tmp4

tosmodeax:
       	jsr    	poplsargs	; Get arguments from stack, adjust sign
      	jsr	udiv32		; Do the division, remainder is in (ptr2:tmp3:tmp4)

; Load the result with the exception of the low byte

  	ldx	ptr2+1
  	ldy    	tmp3
  	sty	sreg
  	ldy	tmp4
  	sty	sreg+1

; Check the sign of the result. It is the sign of the left operand.

        lda     tmp1            ; Check sign of left operand
        bpl     Pos             ; Jump if result is positive

; Result is negative

        lda     ptr2            ; Load byte 0 of result
        jmp     negeax          ; Negate result

; Result is positive

Pos:    lda     ptr2            ; Load byte 0 of result
        rts                     ; Done

