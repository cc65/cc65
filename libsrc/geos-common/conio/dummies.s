;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
; unsigned char __fastcall__ revers (unsigned char onoff);
;

            .export _textcolor, _bgcolor, _bordercolor, _revers
            .import return0

_textcolor      = return0
_bgcolor        = return0
_bordercolor    = return0
_revers         = return0
