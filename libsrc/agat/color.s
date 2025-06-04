;
; Oleg A. Odintsov, Moscow, 2024
;
; unsigned char __fastcall__ textcolor (unsigned char color);
;


	.export		_textcolor
	.include	"agat.inc"


_textcolor:
	ldx	TATTR
	eor	TATTR
	and	#$07
	eor	TATTR
	sta	TATTR
	txa
	and	#$0F
	rts

