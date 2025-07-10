;
; Ullrich von Bassewitz, 08.10.1998
;
; CC65 runtime: += operator for ints on the stack
;

        .export         addeq0sp, addeqysp
        .importzp       c_sp

addeq0sp:
        ldy     #0
addeqysp:
        clc
        adc     (c_sp),y
        sta     (c_sp),y
        pha
        iny
        txa
        adc     (c_sp),y
        sta     (c_sp),y
        tax
        pla
        rts

