;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: -= operator for longs on the stack
;

        .export         lsubeq0sp, lsubeqysp
        .importzp       spc, sreg

lsubeq0sp:
        ldy     #0
lsubeqysp:
        sec
        eor     #$FF
        adc     (spc),y
        sta     (spc),y
        pha                     ; Save low byte
        iny
        txa
        eor     #$FF
        adc     (spc),y
        sta     (spc),y
        tax
        iny
        lda     (spc),y
        sbc     sreg
        sta     (spc),y
        sta     sreg
        iny
        lda     (spc),y
        sbc     sreg+1
        sta     (spc),y
        sta     sreg+1
        pla
        rts

