;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export		_textcolor, _bgcolor, _bordercolor
        .import	       	return0

_bgcolor     := return0

_bordercolor := return0

_textcolor:
	ldx	#$00
	lda	#$03		; COLOR_WHITE
	rts


