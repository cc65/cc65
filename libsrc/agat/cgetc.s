;
; Oleg A. Odintsov, Moscow, 2024
;
; char cgetc (void);
;

	.export         _cgetc
        .import         cursor
	.include        "agat.inc"

_cgetc:
	lda	#$DF ; _
	bit	cursor
	bne	hascur
	lda	#$00
hascur:	sta	CURSOR
	jsr	j1
	cmp	#$A0
	bpl	:+
	and	#$7F
:	rts
j1:	jmp	(VCIN)

