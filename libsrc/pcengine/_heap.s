;
; Ullrich von Bassewitz, 03.06.1998
;
; Heap variables and initialization.
;

; FIXME: there should be a way to configure heap from linkerscript!

       	.constructor	initheap, 24

		.import		__RAM_START__, __RAM_SIZE__, __STACKSIZE__	; Linker generated
		.import __BSS_SIZE__
		.importzp	sp

.data

;; old - remove
     	.export		__horg, __hptr, __hend, __hfirst, __hlast
__horg:
       	.word  	__RAM_START__+__BSS_SIZE__+__DATA_SIZE__	; Linker calculates this symbol
__hptr:
   	.word	__RAM_START__+__BSS_SIZE__+__DATA_SIZE__	; Dito
__hend:
       	.word	__RAM_START__+__RAM_SIZE__
__hfirst:
   	.word	0
__hlast:
   	.word	0

		.export __heaporg
		.export __heapptr
		.export __heapend
		.export __heapfirst
		.export __heaplast

__heaporg:
       	.word  	__RAM_START__+__BSS_SIZE__+__DATA_SIZE__	; Linker calculates this symbol
__heapptr:
       	.word  	__RAM_START__+__BSS_SIZE__+__DATA_SIZE__	; Linker calculates this symbol
__heapend:
       	.word	__RAM_START__+__RAM_SIZE__
__heapfirst:
      	.word	0
__heaplast:
      	.word	0


; Initialization. Will be called from startup!

.code

initheap:
   	;sec
   	;lda	sp
;	lda	#<(__STACKSIZE__)
;	lda #<(__RAM_START__+__RAM_SIZE__)
	lda #<(__RAM_START__+__BSS_SIZE__+__DATA_SIZE__)
	sta	__heapend
	sta	__hend ; old
;	lda	sp+1
;	lda	#>(__STACKSIZE__)
;	lda #>(__RAM_START__+__RAM_SIZE__)
	lda #>(__RAM_START__+__BSS_SIZE__+__DATA_SIZE__)
	sta	__heapend+1
	sta	__hend+1 ; old
	rts

