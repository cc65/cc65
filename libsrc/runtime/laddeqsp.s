;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: += operator for longs on the stack
;

        .export         laddeq0sp, laddeqysp
        .importzp       sp, sreg

laddeq0sp:
        ldy     #0
laddeqysp:
        clc
        adc     (sp),y
        sta     (sp),y
        pha
        iny
        txa
        adc     (sp),y
        sta     (sp),y
        tax
        iny
        lda     sreg
        adc     (sp),y
        sta     (sp),y
        sta     sreg
        iny
        lda     sreg+1
        adc     (sp),y
        sta     (sp),y
        sta     sreg+1
        pla
        rts

