;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
;


        .export         _textcolor, _bgcolor

        .include        "c64.inc"

_textcolor:
        ldx     CHARCOLOR       ; get old value
        sta     CHARCOLOR       ; set new value
        txa
        rts


_bgcolor:
        ldx     VIC_BG_COLOR0   ; get old value
        sta     VIC_BG_COLOR0   ; set new value
        txa
        rts
