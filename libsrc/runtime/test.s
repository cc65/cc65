;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: test int in ax
;

       	.export		utsta0, utstax, tsta0, tstax

tsta0:
utsta0:	ldx	#0
tstax:
utstax:	cpx	#0
      	bne	L1
   	cmp	#0
L1:	rts

  
