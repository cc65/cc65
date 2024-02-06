;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: += operator for ints on the stack
;

        .export         addeq0sp, addeqysp
        .importzp       sp

addeq0sp:
        ldy     #0
addeqysp:
        clc
        adc     (sp),y
        sta     (sp),y
        pha
        iny
        txa
        adc     (sp),y
        sta     (sp),y
        tax
        pla
        rts

