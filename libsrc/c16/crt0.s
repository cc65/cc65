;
; Startup code for cc65 (C16 version)
;
; This must be the *first* file on the linker command line
;
; Note: The C16 is actually the Plus/4 with just 16KB of memory. So many
; things are similar here, and we even use the plus4.inc include file.
;

	.export		_exit
	.import		initlib, donelib
	.import	     	push0, callmain, zerobss
        .import         MEMTOP, RESTOR, BSOUT, CLRCH

        .include        "zeropage.inc"
	.include	"../plus4/plus4.inc"


; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; BASIC header with a SYS call

        .word   Head            ; Load address
Head:   .word   @Next
        .word   .version        ; Line number
        .byte   $9E,"4109"	; SYS 4109
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker

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

; Push arguments and call main()

       	jsr    	callmain

; Call module destructors. This is also the _exit entry.

_exit: 	pha			; Save the return code on stack
	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Store the return code into ST

	pla
	sta	ST

; Reset changed vectors

	jmp	RESTOR


.data
zpsave:	.res	zpspace

.bss
spsave:	.res	1


