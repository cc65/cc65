;
; Ullrich von Bassewitz, 03.06.1998
;
; Heap variables and initialization.
;

     	.export		__horg, __hptr, __hend, __hfirst, __hlast
	.export		__hinit
       	.import	       	__BSS_RUN__, __BSS_SIZE__, __stksize
	.importzp	sp

.data

__horg:
       	.word  	__BSS_RUN__+__BSS_SIZE__	; Linker calculates this symbol
__hptr:
   	.word	__BSS_RUN__+__BSS_SIZE__	; Dito
__hend:
       	.word	__BSS_RUN__+__BSS_SIZE__
__hfirst:		 
   	.word	0
__hlast:
   	.word	0


;
; Initialization. Must be called from startup!
;

.code

__hinit:
   	sec
   	lda	sp
	sbc	__stksize
	sta	__hend
	lda	sp+1
	sbc	__stksize+1
	sta	__hend+1
	rts





