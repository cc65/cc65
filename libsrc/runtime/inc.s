;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by constant or value in Y
;

       	.export	   	incaxy
 	.export	   	incax8, incax7, incax6, incax5
	.export	   	incax4, incax3, incax2, incax1
	.importzp  	tmp1


incax8:	ldy	#8
       	bne 	incaxy

incax7:	ldy	#7
	bne	incaxy

incax6:	ldy	#6
	bne	incaxy

incax5:	ldy	#5
	bne	incaxy

incax4:	ldy	#4
	bne	incaxy

incax3:	ldy	#3
;     	bne	incaxy
incaxy:	sty	tmp1
      	clc
      	adc	tmp1
      	bcc	*+3
      	inx
      	rts
		   
incax2:	clc
	adc	#2
	bcc	*+3
	inx
	rts

incax1:	clc
	adc	#1
	bcc	*+3
	inx
	rts

