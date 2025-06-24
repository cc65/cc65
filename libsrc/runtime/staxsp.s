;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Store ax at (c_sp),y
;

        .export         staxysp, stax0sp
        .importzp       c_sp

stax0sp:
        ldy     #0
staxysp:
        sta     (c_sp),y
        iny
        pha
        txa
        sta     (c_sp),y
        pla
        rts

