;
; Startup code for cc65 (C128 version)
;
; This must be the *first* file on the linker command line
;

    	.export	     	_exit
    	.import	     	condes, initlib, donelib
    	.import	     	zerobss
    	.import		push0, callmain
        .import         RESTOR, BSOUT, CLRCH
	.import	       	__IRQFUNC_TABLE__, __IRQFUNC_COUNT__
    	.import	 	__RAM_START__, __RAM_SIZE__

        .include        "zeropage.inc"
    	.include 	"c128.inc"


; ------------------------------------------------------------------------
; Constants

IRQInd		= $2FD	; JMP $0000 - used as indirect IRQ vector

; ------------------------------------------------------------------------
; Place the startup code in a special segment to cope with the quirks of
; c128 banking.

.segment       	"STARTUP"

; BASIC header with a SYS call

	.org	$1BFF
        .word   Head            ; Load address
Head:   .word   @Next
        .word   .version        ; Line number
        .byte   $9E,"7181"      ; SYS 7181
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker
	.reloc

; ------------------------------------------------------------------------
; Actual code

; Close open files

 	jsr	CLRCH

; Switch to the second charset

 	lda	#14
 	jsr	BSOUT

; Set the bank for the file name our execution bank

        lda     #0
        sta     FNAM_BANK

; Before doing anything else, we have to setup our banking configuration.
; Otherwise just the lowest 16K are actually RAM. Writing through the ROM
; to the underlying RAM works, but it is bad style.

	lda	MMU_CR	 	; Get current memory configuration...
       	pha		 	; ...and save it for later
       	lda    	#MMU_CFG_CC65	; Bank0 with kernal ROM
    	sta	MMU_CR

; Save the zero page locations we need

       	ldx	#zpspace-1
L1:	lda    	sp,x
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

; Push arguments and call main()

NoIRQ1:	jsr	callmain

; Back from main (this is also the _exit entry). Store the return code into
; ST, where it is accessible from BASIC. Reset the IRQ vector if we chained 
; it.

_exit:  sta	ST
	lda     #<__IRQFUNC_COUNT__
	beq	NoIRQ2
	lda	IRQInd+1
	ldx	IRQInd+2
	sei
	sta	IRQVec
	stx	IRQVec+1
	cli

; Run module destructors

NoIRQ2: jsr    	donelib

; Reset the stack

	ldx    	spsave
 	txs

; Copy back the zero page stuff

 	ldx	#zpspace-1
L2:	lda	zpsave,x
     	sta	sp,x
     	dex
     	bpl	L2

; Reset the memory configuration

       	lda    	mmusave
	sta	MMU_CR

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
	ldy 	#<(__IRQFUNC_COUNT__*2)
       	lda    	#<__IRQFUNC_TABLE__
	ldx 	#>__IRQFUNC_TABLE__
	jsr	condes 		   	; Call the functions
	pla    	       			; Get old register value
	sta    	MMU_CR
       	jmp    	IRQInd			; Jump to the saved IRQ vector


; ------------------------------------------------------------------------
; Data

.data
zpsave:	.res	zpspace

.bss
spsave:	.res	1
mmusave:.res	1



