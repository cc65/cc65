;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load a unsigned indirect from address in ax
;

	.export		ldaui, ldauidx
	.importzp	ptr1
	   
ldaui:	
	ldy	#0
ldauidx:
	sta	ptr1
	stx	ptr1+1
	ldx	#0
	lda	(ptr1),y
	rts

