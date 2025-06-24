;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: -= operator for longs on the stack
;

        .export         lsubeq0sp, lsubeqysp
        .importzp       c_sp, sreg

lsubeq0sp:
        ldy     #0
lsubeqysp:
        sec
        eor     #$FF
        adc     (c_sp),y
        sta     (c_sp),y
        pha                     ; Save low byte
        iny
        txa
        eor     #$FF
        adc     (c_sp),y
        sta     (c_sp),y
        tax
        iny
        lda     (c_sp),y
        sbc     sreg
        sta     (c_sp),y
        sta     sreg
        iny
        lda     (c_sp),y
        sbc     sreg+1
        sta     (c_sp),y
        sta     sreg+1
        pla
        rts

