;
; Startup code for cc65 (Apple2 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit, __Exit
	.import	   	initlib, donelib
	.import	   	zerobss
       	.import	       	__STARTUP_LOAD__, __BSS_LOAD__	; Linker generated
	.import		callmain

        .include        "zeropage.inc"
	.include	"apple2.inc"

; ------------------------------------------------------------------------
; The executable header

.segment	"EXEHDR"

       	.word  	__STARTUP_LOAD__                ; Start address
       	.word  	__BSS_LOAD__ - __STARTUP_LOAD__	; Size

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; ProDOS TechRefMan, chapter 5.2.1:
; "For maximum interrupt efficiency, a system program should not use more
;  than the upper 3/4 of the stack."

	ldx	#$FF
	txs	       		; Init stack pointer

; Save the zero page locations we need

       	ldx	#zpspace-1
:	lda	sp,x
   	sta	zpsave,x
	dex
       	bpl	:-

; Save the original RESET vector

	ldx	#$02
:	lda	SOFTEV,x
	sta	rvsave,x
	dex
	bpl	:-

; ProDOS TechRefMan, chapter 5.3.5:
; "Your system program should place in the RESET vector the address of a
;  routine that ... closes the files."

	ldx	#<_exit
	lda	#>_exit
	jsr	reset		; Setup RESET vector
		
; Clear the BSS data

	jsr	zerobss

; Setup the stack

	lda    	HIMEM
	sta	sp
	lda	HIMEM+1
       	sta	sp+1   	    	; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Push arguments and call main()

	jsr	callmain

; Avoid re-entrance of donelib. This is also the _exit entry

_exit:	ldx	#<__Exit
	lda	#>__Exit
	jsr	reset		; Setup RESET vector

; Call module destructors

	jsr	donelib

; Restore the original RESET vector. This is also the __Exit entry

__Exit:	ldx	#$02
:	lda	rvsave,x
	sta	SOFTEV,x
	dex
	bpl	:-

; Copy back the zero page stuff

	ldx	#zpspace-1
:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	:-

; ProDOS TechRefMan, chapter 5.2.1:
; "System programs should set the stack pointer to $FF at the warm-start
;  entry point."

	ldx	#$FF
	txs			; Re-init stack pointer

; Back to DOS

	jmp	DOSWARM

; ------------------------------------------------------------------------
; Setup RESET vector

reset:	stx	SOFTEV
	sta	SOFTEV+1
	eor	#$A5
	sta	PWREDUP
	rts

; ------------------------------------------------------------------------
; Data

.data

zpsave:	.res	zpspace

rvsave:	.res	3
