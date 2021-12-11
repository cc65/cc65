;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: modulo operation for unsigned ints
;

        .export         tosumoda0, tosumodax
        .import         popptr1, udiv16
        .importzp       sreg, ptr4

tosumoda0:
        ldx     #0
tosumodax:
        sta     ptr4
        stx     ptr4+1          ; Save right operand
        jsr     popptr1         ; Get right operand

; Do the division

        jsr     udiv16

; Result is in ptr1, remainder in sreg

        lda     sreg
        ldx     sreg+1
        rts

           
