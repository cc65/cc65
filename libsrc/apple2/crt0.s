;
; Startup code for cc65 (Apple2 version)
;
; This must be the *first* file on the linker command line
;

        .export		_exit, __Exit
        .import		zerobss
        .import	   	initlib, donelib
        .import	   	callmain, callirq
        .import	       	__STARTUP_LOAD__, __BSS_LOAD__	; Linker generated
        .import	       	__INTERRUPTOR_COUNT__		; Linker generated

        .include        "zeropage.inc"
        .include	"apple2.inc"
        .include	"mli.inc"

; ------------------------------------------------------------------------

        .segment	"EXEHDR"

        .addr		__STARTUP_LOAD__                ; Start address
        .word		__BSS_LOAD__ - __STARTUP_LOAD__	; Size

; ------------------------------------------------------------------------

        .segment       	"STARTUP"

        ; ProDOS TechRefMan, chapter 5.2.1:
        ; "For maximum interrupt efficiency, a system program should not
        ;  use more than the upper 3/4 of the stack."
        ldx	#$FF
        txs	       		; Init stack pointer

        ; Delegate all further processing to keep STARTUP small
        jsr	init

        ; Avoid re-entrance of donelib. This is also the _exit entry
_exit:  ldx	#<__Exit
        lda	#>__Exit
        jsr	reset		; Setup RESET vector

        ; Check for valid interrrupt vector table entry number
        lda	intnum
        beq	:+

        ; Deallocate interrupt vector table entry
        dec	params		; Adjust parameter count
        jsr	ENTRY
        .byte	$41		; Dealloc interrupt
        .addr	params

        ; Call module destructors
:       jsr	donelib

        ; Restore the original RESET vector. This is also the __Exit entry
__Exit: ldx	#$02
:       lda	rvsave,x
        sta	SOFTEV,x
        dex
        bpl	:-

        ; Copy back the zero page stuff
        ldx	#zpspace-1
:       lda	zpsave,x
        sta	sp,x
        dex
        bpl	:-

        ; ProDOS TechRefMan, chapter 5.2.1:
        ; "System programs should set the stack pointer to $FF at the
        ;  warm-start entry point."
        ldx	#$FF
        txs			; Re-init stack pointer

        ; Back to DOS
        jmp	DOSWARM

; ------------------------------------------------------------------------

        .segment       	"INIT"

        ; Save the zero page locations we need
init:   ldx	#zpspace-1
:       lda	sp,x
        sta	zpsave,x
        dex
        bpl	:-

        ; Save the original RESET vector
        ldx	#$02
:       lda	SOFTEV,x
        sta	rvsave,x
        dex
        bpl	:-

        ; ProDOS TechRefMan, chapter 5.3.5:
        ; "Your system program should place in the RESET vector the
        ;  address of a routine that ... closes the files."
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

        ; Check for interruptors
        lda     #<__INTERRUPTOR_COUNT__
        beq	:+

        ; Check for ProDOS
        lda	ENTRY
        cmp	#$4C		; Is MLI present? (JMP opcode)
        bne	:+

        ; Allocate interrupt vector table entry
        jsr	ENTRY
        .byte	$40		; Alloc interrupt
        .addr	params

        ; Push arguments and call main()
:       jmp	callmain

; ------------------------------------------------------------------------

        .segment	"LOWCODE"

        ; ProDOS TechRefMan, chapter 6.2:
        ; "Each installed routine must begin with a CLD instruction"
intrpt: cld

        ; Call interruptors
        jsr	callirq

        ; ProDOS TechRefMan, chapter 6.2:
        ; "When the routine that can process the interrupt is called, it
        ;  should ... return (via an RTS) with the carry flag clear."
        clc
        rts

; ------------------------------------------------------------------------

        .code

        ; Setup RESET vector
reset:  stx	SOFTEV
        sta	SOFTEV+1
        eor	#$A5
        sta	PWREDUP
        rts

; ------------------------------------------------------------------------

        .data

zpsave: .res	zpspace

rvsave: .res	3

params: .byte	$02		; Parameter count
intnum: .byte	$00		; Interrupt number
        .addr	intrpt		; Interrupt handler
