;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load word from stack slot and push
;

       	.export		pushwysp, pushw0sp
      	.import		pushax
      	.importzp	sp

pushw0sp:
      	ldy	#1
pushwysp:
      	lda	(sp),y		; get hi byte
      	tax
      	dey
      	lda	(sp),y		; get lo byte
       	jmp	pushax		; push that


