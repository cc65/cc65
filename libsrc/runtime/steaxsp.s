;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Store eax at (c_sp),y
;

        .export         steaxysp, steax0sp
        .importzp       c_sp, sreg

steax0sp:
        ldy     #0
steaxysp:
        sta     (c_sp),y
        iny
        pha
        txa
        sta     (c_sp),y
        iny
        lda     sreg
        sta     (c_sp),y
        iny
        lda     sreg+1
        sta     (c_sp),y
        pla
        rts


