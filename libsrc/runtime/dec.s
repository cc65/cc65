;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Decrement ax by constant or value in Y
;

       	.export	       	decaxy
      	.export		decax2, decax1
      	.importzp	tmp1


decaxy:	sty	tmp1
	sec
	sbc	tmp1
	bcs	*+3
	dex
	rts

decax2:	sec
	sbc	#2
	bcs	*+3
	dex
	rts

decax1:	sec
	sbc	#1
	bcs	*+3
	dex
	rts

			      
