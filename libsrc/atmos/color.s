;
; Ullrich von Bassewitz, 2003-04-13
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         _textcolor, _bgcolor, _bordercolor
        .import         return0, return1

_textcolor      = return1

_bgcolor        = return0

_bordercolor    = return0


