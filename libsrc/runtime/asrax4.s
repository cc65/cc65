;
; Ullrich von Bassewitz, 25.07.2001
;
; CC65 runtime: Scale the primary register by 16
;

      	.export		asrax4
      	.importzp	tmp1

asrax4:	stx	tmp1
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
      	cpx	#$80
      	ror	tmp1
      	ror	a
      	ldx	tmp1
      	rts

