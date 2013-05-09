;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: or on longs
;

        .export         tosor0ax, tosoreax
        .import         addysp1
        .importzp       sp, sreg, tmp1
                                  

tosor0ax:
        ldy     #$00
        sty     sreg
        sty     sreg+1 

tosoreax:
        ldy     #0
        ora     (sp),y          ; byte 0
        sta     tmp1
        iny
        txa
        ora     (sp),y          ; byte 1
        tax
        iny
        lda     sreg
        ora     (sp),y          ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        ora     (sp),y          ; byte 3
        sta     sreg+1

        lda     tmp1
        jmp     addysp1

