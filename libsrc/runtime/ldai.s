;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load a indirect from address in ax
;

	.export		ldai, ldaidx
	.importzp	ptr1

ldai:	ldy	#$00
ldaidx:	sta	ptr1
	stx	ptr1+1
	ldx	#$00
	lda	(ptr1),y
	bpl	L9
	dex
L9:	rts

