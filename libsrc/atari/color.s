;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


 	.export		_textcolor, _bgcolor, _bordercolor

	.include	"atari.inc"

_textcolor:
	ldx	COLOR1	; get old value
 	sta	COLOR1	; set new value
	txa
	rts


_bgcolor:
	ldx	COLOR2	; get old value
	sta	COLOR2	; set new value
	txa
	rts


_bordercolor:
	ldx	COLOR4	; get old value
	sta	COLOR4	; set new value
	txa
	rts

