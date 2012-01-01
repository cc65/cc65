;
; Startup code for cc65 (Vic20 version)
;

	.export		_exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
	.import		initlib, donelib, callirq
       	.import	       	zerobss, push0
	.import	     	callmain
        .import         RESTOR, BSOUT, CLRCH
	.import	       	__INTERRUPTOR_COUNT__
     	.import		__RAM_START__, __RAM_SIZE__	; Linker generated
        .import		__STACKSIZE__			; Linker generated

        .include        "zeropage.inc"
     	.include     	"vic20.inc"

; ------------------------------------------------------------------------
; Startup code

.segment       	"STARTUP"

Start:

; Save the zero page locations we need

        ldx    	#zpspace-1
L1:	lda	sp,x
   	sta	zpsave,x
	dex
       	bpl	L1

; Switch to second charset

	lda	#14
	jsr	BSOUT

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

       	tsx
       	stx    	spsave 		; Save the system stack ptr

	lda    	#<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
       	sta	sp+1   		; Set argument stack ptr

; If we have IRQ functions, chain our stub into the IRQ vector

        lda     #<__INTERRUPTOR_COUNT__
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

; Call module constructors

NoIRQ1: jsr     initlib

; Push arguments and call main()

        jsr     callmain

; Back from main (This is also the _exit entry). Run module destructors

_exit:  jsr     donelib

; Reset the IRQ vector if we chained it.

        pha  		     	; Save the return code on stack
	lda     #<__INTERRUPTOR_COUNT__
	beq	NoIRQ2
	lda	IRQInd+1
	ldx	IRQInd+2
	sei
	sta	IRQVec
	stx	IRQVec+1
	cli

; Copy back the zero page stuff

NoIRQ2: ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Place the program return code into ST

	pla
	sta	ST

; Restore the stack pointer

  	ldx	spsave
	txs

; Back to basic

        rts


; ------------------------------------------------------------------------
; The IRQ vector jumps here, if condes routines are defined with type 2.

IRQStub:
	cld    	       		   	; Just to be sure
       	jsr    	callirq                 ; Call the functions
       	jmp    	IRQInd			; Jump to the saved IRQ vector

; ------------------------------------------------------------------------
; Data

.data

IRQInd: jmp     $0000

.segment        "ZPSAVE"

zpsave:	.res	zpspace

.bss

spsave:	.res	1
