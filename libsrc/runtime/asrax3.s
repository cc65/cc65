;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: Scale the primary register by 8
;

	.export		asrax3
	.importzp	tmp1

asrax3:	stx	tmp1
      	cpx	#$80  		; Put bit 7 into carry
      	ror	tmp1
      	ror	a
	ldx	tmp1
      	cpx	#$80
      	ror	tmp1
      	ror	a
	ldx	tmp1
      	cpx	#$80
      	ror	tmp1
      	ror	a
      	ldx	tmp1
      	rts

