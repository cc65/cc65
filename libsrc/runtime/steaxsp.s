;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Store eax at (sp),y
;

        .export         steaxysp, steax0sp
        .importzp       sp, sreg

steax0sp:
        ldy     #0
steaxysp:
        sta     (sp),y
        iny
        pha
        txa
        sta     (sp),y
        iny
        lda     sreg
        sta     (sp),y
        iny
        lda     sreg+1
        sta     (sp),y
        pla
        rts


