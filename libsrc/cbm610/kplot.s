;
; Ullrich von Bassewitz, 28.09.1998
;
; PLOT routine for the 610.
;

      	.export		PLOT
	.importzp	crtc

      	.include      	"cbm610.inc"


.proc	PLOT

	bcc	set
	ldx	CURS_Y
	ldy	CURS_X
	rts

set:   	stx	CURS_Y
	sty	CURS_X
	lda	LineLSBTab,x
	sta	CharPtr
	lda	LineMSBTab,x
	sta	CharPtr+1

	lda	IndReg
	pha
	lda	#$0F
	sta	IndReg

	ldy	#$00
	clc
	sei
	sta	(crtc),y
	lda	CharPtr
	adc	CURS_X
	iny
	sta	(crtc),y
	dey
	lda	#$0E
	sta	(crtc),y
	iny
	lda	(crtc),y
	and	#$F8
	sta	sedt1
	lda	CharPtr+1
	adc	#$00
	and	#$07
	ora	sedt1
	sta	(crtc),y
	cli

	pla
	sta	IndReg
	rts
.endproc

; -------------------------------------------------------------------------
; Low bytes of the start address of the screen lines

.rodata

LineLSBTab:
        .byte   $00,$50,$A0,$F0,$40,$90,$E0,$30
        .byte   $80,$D0,$20,$70,$C0,$10,$60,$B0
        .byte   $00,$50,$A0,$F0,$40,$90,$E0,$30
        .byte   $80
; -------------------------------------------------------------------------
; High bytes of the start address of the screen lines

LineMSBTab:
        .byte   $D0,$D0,$D0,$D0,$D1,$D1,$D1,$D2
        .byte   $D2,$D2,$D3,$D3,$D3,$D4,$D4,$D4
        .byte   $D5,$D5,$D5,$D5,$D6,$D6,$D6,$D7
        .byte   $D7
