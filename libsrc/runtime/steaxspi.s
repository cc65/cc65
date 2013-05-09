;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Store eax at the address on top of stack with index
;


        .export         steaxspidx
        .import         popax
        .importzp       sreg, ptr1, tmp1, tmp2, tmp3


.proc   steaxspidx

        sta     tmp1
        stx     tmp2
        sty     tmp3
        jsr     popax           ; get the pointer
        sta     ptr1
        stx     ptr1+1
        ldy     tmp3
        lda     tmp1
        sta     (ptr1),y
        iny
        lda     tmp2
        sta     (ptr1),y
        iny
        tax
        lda     sreg
        sta     (ptr1),y
        iny
        lda     sreg+1
        sta     (ptr1),y
        lda     tmp1
        rts

.endproc


