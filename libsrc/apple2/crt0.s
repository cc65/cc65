;
; Startup code for cc65 (Apple2 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
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
L1:	lda	sp,x
   	sta	zpsave,x
	dex
       	bpl	L1

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

	lda    	HIMEM
	sta	sp
	lda	HIMEM+1
       	sta	sp+1   	    	; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Push arguments and call main()

	jsr	callmain

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; ProDOS TechRefMan, chapter 5.2.1:
; "System programs should set the stack pointer to $FF at the warm-start
;  entry point."

	ldx	#$FF
	txs			; Re-init stack pointer

; Back to DOS

	jmp	DOSWARM

; ------------------------------------------------------------------------
; Data

.data

zpsave:	.res	zpspace
