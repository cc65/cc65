;
; Ullrich von Bassewitz, 21.08.1998
;
; CC65 runtime: Load effective address with offset in Y relative to SP
;

    	.export		lea0sp, leaysp, plea0sp, pleaysp
	.import		pushax
	.importzp	sp

lea0sp:	ldy	#0   		; Load offset zero
leaysp:	tya
	ldx	sp+1		; Get high byte
 	clc
	adc	sp
   	bcc	L8
   	inx
L8:	rts


plea0sp:
	ldy	#0
pleaysp:
  	tya
  	ldx	sp+1		; Get high byte
  	clc
  	adc	sp
   	bcc	L9
   	inx
L9:	jmp	pushax



