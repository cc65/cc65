;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


 	    .export _textcolor, _bgcolor, _bordercolor

; for GEOS 2.0 there is no color support, perhaps Wheels has it

_textcolor:
_bgcolor:
_bordercolor:
	rts
