;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Decrement eax by value in Y
;

        .export         deceaxy
        .importzp       sreg, tmp1

deceaxy:
        sty     tmp1
        sec
        sbc     tmp1
        sta     tmp1
        txa
        sbc     #0
        tax
        lda     sreg
        sbc     #0
        sta     sreg
        lda     sreg+1
        sbc     #0
        sta     sreg+1
        lda     tmp1
        rts

