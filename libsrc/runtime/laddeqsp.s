;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: += operator for longs on the stack
;

        .export         laddeq0sp, laddeqysp
        .importzp       spc, sreg

laddeq0sp:
        ldy     #0
laddeqysp:
        clc
        adc     (spc),y
        sta     (spc),y
        pha
        iny
        txa
        adc     (spc),y
        sta     (spc),y
        tax
        iny
        lda     sreg
        adc     (spc),y
        sta     (spc),y
        sta     sreg
        iny
        lda     sreg+1
        adc     (spc),y
        sta     (spc),y
        sta     sreg+1
        pla
        rts

