;
; Startup code for cc65 (PET version)
;
; This must be the *first* file on the linker command line
;

	.export	      	_exit
	.import		initlib, donelib
       	.import	       	zerobss, push0
	.import		_main
        .import         CLRCH, BSOUT

        .include        "zeropage.inc"
	.include	"pet.inc"
	.include	"../cbm/cbm.inc"

; ------------------------------------------------------------------------
; Create an empty LOWCODE segment to avoid linker warnings

.segment        "LOWCODE"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; BASIC header with a SYS call

        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
        .byte   $9E,"1037"      ; SYS 1037
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker

; ------------------------------------------------------------------------
; Actual code

       	ldx	#zpspace-1
L1:	lda	sp,x
      	sta	zpsave,x	; Save the zero page locations we need
	dex
       	bpl	L1

; Close open files

	jsr	CLRCH

; Switch to second charset

	lda	#14
;	sta	$E84C		; See PET FAQ
	jsr	BSOUT

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

       	tsx
       	stx    	spsave 		; Save the system stack ptr

	lda    	MEMSIZE
	sta	sp
	lda	MEMSIZE+1
       	sta	sp+1   	 	; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Pass an empty command line

	jsr	push0 	  	; argc
	jsr	push0	 	; argv

	ldy	#4  	 	; Argument size
       	jsr    	_main	 	; call the users code

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs	    	 	; Restore stack pointer

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Back to basic

       	rts


; ------------------------------------------------------------------------
; Data

.data

zpsave:	.res	zpspace

.bss

spsave:	.res	1
mmusave:.res	1

