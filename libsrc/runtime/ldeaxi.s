;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load eax indirect from address in ax
;

        .export         ldeaxidx, ldeaxi
        .importzp       sreg, ptr1

ldeaxi: ldy     #3
ldeaxidx:
        sta     ptr1
        stx     ptr1+1
        lda     (ptr1),y
        dey
        sta     sreg+1
        lda     (ptr1),y
        dey
        sta     sreg
        lda     (ptr1),y
        dey
        tax
        lda     (ptr1),y
        rts

