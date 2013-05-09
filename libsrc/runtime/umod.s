;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: modulo operation for unsigned ints
;

        .export         tosumoda0, tosumodax
        .import         popsreg, udiv16
        .importzp       ptr1, ptr4

tosumoda0:
        ldx     #0
tosumodax:
        sta     ptr4
        stx     ptr4+1          ; Save right operand
        jsr     popsreg         ; Get right operand

; Do the division

        jsr     udiv16

; Result is in sreg, remainder in ptr1

        lda     ptr1
        ldx     ptr1+1
        rts

           
