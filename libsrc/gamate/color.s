;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


        .export     _textcolor, _bgcolor, _bordercolor

        .include        "gamate.inc"
        .include        "extzp.inc"

_textcolor:
        ldx     CHARCOLOR       ; get old value
        sta     CHARCOLOR       ; set new value
        txa
        rts

_bgcolor:
        ldx     BGCOLOR         ; get old value
        sta     BGCOLOR         ; set new value
        txa
        rts

_bordercolor:
        lda     #0
        tax
        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
