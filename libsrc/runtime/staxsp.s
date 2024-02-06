;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Store ax at (sp),y
;

        .export         staxysp, stax0sp
        .importzp       sp

stax0sp:
        ldy     #0
staxysp:
        sta     (sp),y
        iny
        pha
        txa
        sta     (sp),y
        pla
        rts

