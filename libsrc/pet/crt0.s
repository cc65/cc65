;
; Startup code for cc65 (PET version)
;
; This must be the *first* file on the linker command line
;

	.export	      	_exit
	.import		initlib, donelib
       	.import	       	zerobss, push0
	.import		callmain
        .import         CLRCH, BSOUT

        .include        "zeropage.inc"
	.include	"pet.inc"
	.include	"../cbm/cbm.inc"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; BASIC header with a SYS call

        .word   Head            ; Load address
Head:   .word   @Next
        .word   .version        ; Line number
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

; Push arguments and call main()

	jsr	callmain

; Call module destructors. This is also the _exit entry.

_exit: 	pha			; Save the return code on stack
	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs	    	 	; Restore stack pointer

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Store the program return code into ST

	pla
	sta	ST

; Back to basic

       	rts


; ------------------------------------------------------------------------
; Data

.data

zpsave:	.res	zpspace

.bss

spsave:	.res	1
mmusave:.res	1

