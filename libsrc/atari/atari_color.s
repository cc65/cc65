;
; Christian Groessler, 13-Dec-2002
;
; unsigned char __fastcall__ _atari_textcolor (unsigned char color);
; unsigned char __fastcall__ _atari_bgcolor (unsigned char color);
; unsigned char __fastcall__ _atari_bordercolor (unsigned char color);
;


 	.export		__atari_textcolor, __atari_bgcolor, __atari_bordercolor

	.include	"atari.inc"

__atari_textcolor:
	ldx	COLOR1	; get old value
 	sta	COLOR1	; set new value
	txa
	rts


__atari_bgcolor:
	ldx	COLOR2	; get old value
	sta	COLOR2	; set new value
	txa
	rts


__atari_bordercolor:
	ldx	COLOR4	; get old value
	sta	COLOR4	; set new value
	txa
	rts

