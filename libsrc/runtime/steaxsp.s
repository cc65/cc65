;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Store eax at (spc),y
;

        .export         steaxysp, steax0sp
        .importzp       spc, sreg

steax0sp:
        ldy     #0
steaxysp:
        sta     (spc),y
        iny
        pha
        txa
        sta     (spc),y
        iny
        lda     sreg
        sta     (spc),y
        iny
        lda     sreg+1
        sta     (spc),y
        pla
        rts


