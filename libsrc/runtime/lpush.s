;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: long push
;

;
; push eax on stack
;
        .export         pushl0, push0ax, pusheax
        .import         decsp4
        .importzp       sp, sreg

pushl0:
        lda     #0
        tax
push0ax:
        ldy     #0
        sty     sreg
        sty     sreg+1
pusheax:
        pha                     ; decsp will destroy A (but not X)
        jsr     decsp4
        ldy     #3
        lda     sreg+1
        sta     (sp),y
        dey
        lda     sreg
        sta     (sp),y
        dey
        txa
        sta     (sp),y
        dey
        pla
        sta     (sp),y
        rts

