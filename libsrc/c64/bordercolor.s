;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


        .export         _bordercolor

        .include        "c64.inc"

_bordercolor:
        ldx     VIC_BORDERCOLOR ; get old value
        sta     VIC_BORDERCOLOR ; set new value
        txa
        rts

