;
; Ullrich von Bassewitz, 21.08.1998
;
; CC65 runtime: Load effective address with offset in A relative to SP and push it
;

    	.export		pleaasp
     	.import		pushax
     	.importzp	sp

.proc	pleaasp

  	ldx	sp+1		; Get high byte
  	clc
  	adc	sp
   	bcc	@L9
   	inx
@L9:	jmp	pushax

.endproc



