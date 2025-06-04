;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Store ax at (spc),y
;

        .export         staxysp, stax0sp
        .importzp       spc

stax0sp:
        ldy     #0
staxysp:
        sta     (spc),y
        iny
        pha
        txa
        sta     (spc),y
        pla
        rts

