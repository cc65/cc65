;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

 	.export		_textcolor, _bgcolor, _bordercolor

	.include	"c128.inc"


_textcolor:
	bit	MODE		; Check 80/40 column mode
	bpl	@L1		; Jump if 40 columns
	tax
	lda	$CE5C,x		; Translate VIC color -> VDC color
@L1:	ldx	CHARCOLOR	; get old value
 	sta	CHARCOLOR	; set new value
	txa
	rts


_bgcolor:
	ldx	VIC_BG_COLOR0	; get old value
	sta	VIC_BG_COLOR0	; set new value
	txa
	rts


_bordercolor:
       	ldx    	VIC_BORDERCOLOR	; get old value
	sta	VIC_BORDERCOLOR	; set new value
	txa
	rts

