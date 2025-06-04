;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: += operator for ints on the stack
;

        .export         addeq0sp, addeqysp
        .importzp       spc

addeq0sp:
        ldy     #0
addeqysp:
        clc
        adc     (spc),y
        sta     (spc),y
        pha
        iny
        txa
        adc     (spc),y
        sta     (spc),y
        tax
        pla
        rts

