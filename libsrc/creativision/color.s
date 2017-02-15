;*
;* unsigned char __fastcall__ textcolor (unsigned char color);
;* unsigned char __fastcall__ bgcolor (unsigned char color);
;* unsigned char __fastcall__ bordercolor (unsigned char color);
;*

        .export         _textcolor, _bgcolor, _bordercolor
        .import         return0
        .include        "creativision.inc"

_bordercolor    =       return0;
_textcolor      =       return0;
_bgcolor        =       return0;
