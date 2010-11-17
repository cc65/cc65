;
; Startup code for cc65 (C128 version)
;

    	.export	     	_exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
    	.import	     	callirq, initlib, donelib
    	.import	     	zerobss
    	.import		push0, callmain
        .import         RESTOR, BSOUT, CLRCH
	.import	       	__INTERRUPTOR_COUNT__
    	.import	 	__RAM_START__, __RAM_SIZE__, __STACKSIZE__

        .include        "zeropage.inc"
    	.include 	"c128.inc"


; ------------------------------------------------------------------------
; Constants

IRQInd	      	= $2FD	; JMP $0000 - used as indirect IRQ vector

; ------------------------------------------------------------------------
; Startup code

.segment       	"STARTUP"

Start:

; Switch to the second charset

      	lda	#14
      	jsr	BSOUT

; Before doing anything else, we have to setup our banking configuration.
; Otherwise just the lowest 16K are actually RAM. Writing through the ROM
; to the underlying RAM works, but it is bad style.

      	lda	MMU_CR	 	; Get current memory configuration...
       	pha		 	; ...and save it for later
       	lda    	#MMU_CFG_CC65	; Bank0 with kernal ROM
      	sta	MMU_CR

; Save the zero page locations we need

       	ldx	#zpspace-1
L1:   	lda    	sp,x
      	sta	zpsave,x
      	dex
       	bpl	L1

; Clear the BSS data

      	jsr	zerobss

; Save system stuff and setup the stack

      	pla		 	; Get MMU setting
      	sta	mmusave

       	tsx
       	stx    	spsave	 	; Save the system stack pointer

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

; Set the bank for the file name to our execution bank. We must do this,
; *after* calling constructors, because some of them may depend on the
; original value of this register.

        lda     #0
        sta     FNAM_BANK

; Push arguments and call main()

        jsr     callmain

; Back from main (this is also the _exit entry). Run module destructors

_exit:  jsr     donelib

; Reset the IRQ vector if we chained it.

        pha		     		; Save the return code on stack
	lda     #<__INTERRUPTOR_COUNT__
	beq	NoIRQ2
	lda	IRQInd+1
	ldx	IRQInd+2
	sei
	sta	IRQVec
	stx	IRQVec+1
	cli

; Copy back the zero page stuff

NoIRQ2: ldx     #zpspace-1
L2:	lda	zpsave,x
     	sta	sp,x
     	dex
     	bpl	L2

; Place the program return code into ST

	pla
	sta	ST

; Reset the stack and the memory configuration

	ldx    	spsave
 	txs
       	ldx    	mmusave
	stx	MMU_CR

; Done, restore kernal vectors in an attempt to cleanup

     	jmp	RESTOR

; ------------------------------------------------------------------------
; The C128 has ROM parallel to the RAM starting from $4000. The startup code
; above will change this setting so that we have RAM from $0000-$BFFF. This
; works quite well with the exception of interrupts: The interrupt handler
; is in ROM, and the ROM switches back to the ROM configuration, which means
; that parts of our program may not be accessible. To solve this, we place
; the following code into a special segment called "LOWCODE" which will be
; placed just above the startup code, so it goes into a RAM area that is
; not banked.

.segment        "LOWCODE"

IRQStub:
	cld    	       		   	; Just to be sure
	lda     MMU_CR 		   	; Get old register value
	pha    	       		   	; And save on stack
	lda     #MMU_CFG_CC65		; Bank 0 with kernal ROM
	sta    	MMU_CR
       	jsr    	callirq                 ; Call the functions
	pla    	       			; Get old register value
	sta    	MMU_CR
       	jmp    	IRQInd			; Jump to the saved IRQ vector


; ------------------------------------------------------------------------
; Data

.segment        "ZPSAVE"

zpsave:	.res	zpspace

.bss
spsave:	.res	1
mmusave:.res	1



