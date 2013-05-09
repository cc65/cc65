;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: and on longs
;

        .export         tosand0ax, tosandeax
        .import         addysp1
        .importzp       sp, sreg, tmp1

                                   
tosand0ax:
        ldy     #$00
        sty     sreg
        sty     sreg+1

tosandeax:
        ldy     #0
        and     (sp),y          ; byte 0
        sta     tmp1
        iny
        txa
        and     (sp),y          ; byte 1
        tax
        iny
        lda     sreg
        and     (sp),y          ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        and     (sp),y          ; byte 3
        sta     sreg+1

        lda     tmp1
        jmp     addysp1

