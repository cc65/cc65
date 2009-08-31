;
; Ullrich von Bassewitz, 21.08.1998
;
; CC65 runtime: Load effective address with offset in A relative to SP
;

    	.export		leaasp
     	.importzp	sp

.proc	leaasp

	ldx	sp+1	      	; Get high byte
     	clc
     	adc	sp
     	bcc	@L9
     	inx
@L9: 	rts

.endproc

