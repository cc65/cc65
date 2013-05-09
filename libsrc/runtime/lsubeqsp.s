;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: -= operator for longs on the stack
;

        .export         lsubeq0sp, lsubeqysp
        .importzp       sp, sreg

lsubeq0sp:
        ldy     #0
lsubeqysp:
        sec
        eor     #$FF
        adc     (sp),y
        sta     (sp),y
        pha                     ; Save low byte
        iny
        txa
        eor     #$FF
        adc     (sp),y
        sta     (sp),y
        tax
        iny
        lda     (sp),y
        sbc     sreg
        sta     (sp),y
        sta     sreg
        iny
        lda     (sp),y
        sbc     sreg+1
        sta     (sp),y
        sta     sreg+1
        pla
        rts

