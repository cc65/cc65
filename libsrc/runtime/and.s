;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: and on ints
;

       	.export		tosanda0, tosandax
	.import		addysp1
	.importzp	sp, ptr4

tosanda0:
      	ldx	#$00
tosandax:
	ldy	#0
       	and	(sp),y
	sta	ptr4
	iny
	txa
	and	(sp),y
	tax
	lda	ptr4
	jmp	addysp1		; drop TOS, set condition codes

