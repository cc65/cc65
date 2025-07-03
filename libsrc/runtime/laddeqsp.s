;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: += operator for longs on the stack
;

        .export         laddeq0sp, laddeqysp
        .importzp       c_sp, sreg

laddeq0sp:
        ldy     #0
laddeqysp:
        clc
        adc     (c_sp),y
        sta     (c_sp),y
        pha
        iny
        txa
        adc     (c_sp),y
        sta     (c_sp),y
        tax
        iny
        lda     sreg
        adc     (c_sp),y
        sta     (c_sp),y
        sta     sreg
        iny
        lda     sreg+1
        adc     (c_sp),y
        sta     (c_sp),y
        sta     sreg+1
        pla
        rts

