;
; Ullrich von Bassewitz, 06.08.1998
;
; Low level stuff for screen output/console input
;

   	.export		initconio, doneconio
	.exportzp	CURS_X, CURS_Y
	.import		xsize, ysize

	.include	"c128.inc"
	.include	"../cbm/cbm.inc"

.bss
keyvec:	.res	2


.code

initconio:

; Save the old vector

	lda	KeyStoreVec
	sta	keyvec
	lda	KeyStoreVec+1
	sta	keyvec+1

; Set the new vector. I can only hope that this works for other C128
; versions...

	lda	#<$C6B7
	ldx	#>$C6B7

SetVec:	sei
	sta	KeyStoreVec
	stx	KeyStoreVec+1
	cli
	rts

doneconio:
	lda	keyvec
	ldx	keyvec+1
	bne	SetVec

