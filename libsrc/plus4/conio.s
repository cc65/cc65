;
; Ullrich von Bassewitz, 06.08.1998
;
; Low level stuff for screen output/console input
;

   	.export		initconio, doneconio
	.exportzp	CURS_X, CURS_Y
	.import		xsize, ysize

	.include	"plus4.inc"
	.include	"../cbm/cbm.inc"

.code

initconio:
     	jsr	SCREEN
     	stx	xsize
     	sty	ysize
     	ldy	#15
L1:  	lda	fnkeys,y
     	sta 	FKEY_SPACE,y
     	dey
     	bpl	L1
     	rts


doneconio:
     	ldx	#$39		; Copy the original function keys
L2:  	lda	FKEY_ORIG,x
     	sta	FKEY_SPACE,x
     	dex
     	bpl	L2
     	rts

; Function key table, readonly

.rodata
fnkeys:	.byte	$01, $01, $01, $01, $01, $01, $01, $01
     	.byte	133, 137, 134, 138, 135, 139, 136, 140

