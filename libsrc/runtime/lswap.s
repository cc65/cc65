;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Exchange lo and hi part of eax
;

	.export		swapeax
	.importzp	sreg

swapeax:
	ldy	sreg
       	sta	sreg
	lda	sreg+1
	stx	sreg+1
	tax
	tya
	rts

