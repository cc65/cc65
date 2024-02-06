;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         _textcolor, _bgcolor, _bordercolor

        .include        "plus4.inc"

_textcolor:
        ldx     CHARCOLOR       ; get old value
        sta     CHARCOLOR       ; set new value
        txa
        rts


_bgcolor:
        ldx     TED_BGCOLOR     ; get old value
        sta     TED_BGCOLOR     ; set new value
        txa
        rts


_bordercolor:
        ldx     TED_BORDERCOLOR ; get old value
        sta     TED_BORDERCOLOR ; set new value
        txa
        rts


