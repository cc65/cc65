;
; Startup code for cc65 (Plus/4 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		initlib, donelib
	.import	     	push0, _main, zerobss

        .include        "zeropage.inc"
	.include	"plus4.inc"
	.include	"../cbm/cbm.inc"

; ------------------------------------------------------------------------
; BASIC header with a SYS call

.code

	.org	$0FFF
        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
        .byte   $9E,"4109"	; SYS 4109
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker
	.reloc

; ------------------------------------------------------------------------
; Actual code

       	ldx   	#zpspace-1
L1:	lda	sp,x
   	sta	zpsave,x	; save the zero page locations we need
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
       	stx    	spsave       	; save system stk ptr

	sec
	jsr	MEMTOP		; Get top memory
	cpy	#$80  		; We can only use the low 32K :-(
	bcc	MemOk
	ldy	#$80
	ldx	#$00
MemOk:	stx	sp
      	sty	sp+1  		; set argument stack ptr

; Call module constructors

	jsr	initlib

; Pass an empty command line

       	jsr    	push0		; argc
	jsr	push0		; argv

	ldy	#4    		; Argument size
       	jsr    	_main 		; call the users code

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Reset changed vectors

	jmp	RESTOR


.data
zpsave:	.res	zpspace

.bss
spsave:	.res	1


