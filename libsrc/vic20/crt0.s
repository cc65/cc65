;
; Startup code for cc65 (Vic20 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		initlib, donelib
       	.import	       	zerobss, push0
	.import	     	_main
	.import		__RAM_START__, __RAM_SIZE__	; Linker generated

        .include        "zeropage.inc"
	.include     	"vic20.inc"
	.include     	"../cbm/cbm.inc"

.code

; ------------------------------------------------------------------------
; BASIC header with a SYS call

	.org	$FFF
        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
        .byte   $9E,"4109"      ; SYS 4109
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker
	.reloc

; ------------------------------------------------------------------------
; Actual code

	ldx    	#zpspace-1
L1:	lda	sp,x
   	sta	zpsave,x	; Save the zero page locations we need
	dex
       	bpl	L1

; Close open files

	jsr	CLRCH

; Switch to second charset

	lda	#14
	jsr	BSOUT

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

       	tsx
       	stx    	spsave 		; Save the system stack ptr

	lda    	#<(__RAM_START__ + __RAM_SIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__)
       	sta	sp+1   		; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Pass an empty command line

       	jsr    	push0 	  	; argc
	jsr	push0	  	; argv

	ldy	#4	  	; Argument size
       	jsr    	_main	  	; call the users code

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Restore system stuff

  	ldx	spsave
	txs	   	  	; Restore stack pointer

; Copy back the zero page stuff

       	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Reset changed vectors, back to basic

	jmp	RESTOR


.data

zpsave:	.res	zpspace

.bss

spsave:	.res	1
