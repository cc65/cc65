;
; Ullrich von Bassewitz, 03.06.1998
;
; Heap variables and initialization.
;

     	.export		__horg, __hptr, __hend, __hfirst, __hlast
       	.constructor	initheap, 24
       	.import	       	__BSS_RUN__, __BSS_SIZE__, __STACKSIZE__
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


; Initialization. Will be called from startup!

.code

initheap:
   	sec
   	lda	sp
	sbc	#<__STACKSIZE__
	sta	__hend
	lda	sp+1
	sbc	#>__STACKSIZE__
	sta	__hend+1
	rts

