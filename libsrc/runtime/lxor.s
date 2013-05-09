;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: xor on longs
;

        .export         tosxor0ax, tosxoreax
        .import         addysp1
        .importzp       sp, sreg, tmp1

tosxor0ax:
        ldy     #$00
        sty     sreg
        sty     sreg+1

tosxoreax:                         
        ldy     #0
        eor     (sp),y          ; byte 0
        sta     tmp1
        iny
        txa
        eor     (sp),y          ; byte 1
        tax
        iny
        lda     sreg
        eor     (sp),y          ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        eor     (sp),y          ; byte 3
        sta     sreg+1

        lda     tmp1
        jmp     addysp1



