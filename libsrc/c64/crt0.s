;
; Startup code for cc65 (C64 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		initlib, donelib, callirq
       	.import	       	zerobss, push0
	.import	     	callmain
        .import         RESTOR, BSOUT, CLRCH
	.import	       	__IRQFUNC_COUNT__
	.import		__RAM_START__, __RAM_SIZE__	; Linker generated

        .include        "zeropage.inc"
	.include     	"c64.inc"


; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; BASIC header with a SYS call

        .word   Head            ; Load address
Head:   .word   @Next
        .word   .version        ; Line number
        .byte   $9E,"2061"      ; SYS 2061
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker

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

; Switch off the BASIC ROM

	lda	$01
       	pha                     ; Remember the value
	and	#$F8
       	ora	#$06		; Enable kernal+I/O, disable basic
	sta	$01

; Clear the BSS data

	jsr	zerobss

; Save system settings and setup the stack

        pla
        sta	mmusave      	; Save the memory configuration

       	tsx
       	stx    	spsave 		; Save the system stack ptr

	lda    	#<(__RAM_START__ + __RAM_SIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__)
       	sta	sp+1   		; Set argument stack ptr

; Call module constructors

	jsr	initlib

; If we have IRQ functions, chain our stub into the IRQ vector

        lda     #<__IRQFUNC_COUNT__
      	beq	NoIRQ1
      	lda	IRQVec
       	ldx	IRQVec+1
      	sta	IRQInd+1
      	stx	IRQInd+2
      	lda	#<IRQStub
      	ldx	#>IRQStub
      	sei
      	sta	IRQVec
      	stx	IRQVec+1
      	cli

; Push arguments and call main

NoIRQ1: jsr     callmain

; Back from main (This is also the _exit entry). Reset the IRQ vector if we
; chained it.

_exit: 	pha  			; Save the return code on stack
	lda     #<__IRQFUNC_COUNT__
	beq	NoIRQ2
	lda	IRQInd+1
	ldx	IRQInd+2
	sei
	sta	IRQVec
	stx	IRQVec+1
	cli

; Run module destructors

NoIRQ2: jsr	donelib

; Copy back the zero page stuff

       	ldx	#zpspace-1
L2:   	lda	zpsave,x
      	sta	sp,x
      	dex
       	bpl	L2

; Place the program return code into ST

	pla
	sta	ST

; Restore system stuff

  	ldx	spsave
	txs	       	  	; Restore stack pointer
       	ldx    	mmusave
	stx	$01    	  	; Restore memory configuration

; Reset changed vectors, back to basic

	jmp	RESTOR

; ------------------------------------------------------------------------
; The IRQ vector jumps here, if condes routines are defined with type 2.

IRQStub:
	cld    	       		   	; Just to be sure
       	jsr    	callirq                 ; Call the functions
       	jmp    	IRQInd			; Jump to the saved IRQ vector

; ------------------------------------------------------------------------
; Data

.data

zpsave:	.res	zpspace
IRQInd: jmp     $0000

.bss

spsave:	.res	1
mmusave:.res	1
