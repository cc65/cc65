;
; Startup code for cc65 (C128 version)
;
; This must be the *first* file on the linker command line
;

    	.export	     	_exit
    	.export	     	BRKStub, BRKOld, BRKInd
    	.import	     	condes, initlib, donelib
    	.import	     	initconio, doneconio, zerobss
    	.import		push0, _main
	.import	       	__IRQFUNC_TABLE__, __IRQFUNC_COUNT__
    	.import	 	__RAM_START__, __RAM_SIZE__

    	.include 	"c128.inc"
	.include 	"../cbm/cbm.inc"

; ------------------------------------------------------------------------
; Constants

IRQInd		= $2FD	; JMP $0000 - used as indirect IRQ vector

; ------------------------------------------------------------------------
; Define and export the ZP variables for the C64 runtime

	.exportzp	sp, sreg, regsave
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4
	.exportzp	regbank, zpspace

.zeropage

zpstart	= *
sp:	      	.res   	2 	; Stack pointer
sreg:	      	.res	2	; Secondary register/high 16 bit for longs
regsave:      	.res	2	; slot to save/restore (E)AX into
ptr1:	      	.res	2
ptr2:	      	.res	2
ptr3:	      	.res	2
ptr4:	      	.res	2
tmp1:	      	.res	1
tmp2:	      	.res	1
tmp3:	      	.res	1
tmp4:	      	.res	1
regbank:      	.res	6	; 6 byte register bank

zpspace	= * - zpstart	 	; Zero page space allocated

; Place the startup code in a special segment to cope with the quirks of
; c128 banking. Do also create an empty segment named "NMI" to avoid
; warnings if the rs232 routines are not used.

.segment        "NMI"
; empty

.segment       	"STARTUP"

; ------------------------------------------------------------------------
; BASIC header with a SYS call

	.org	$1BFF
        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
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

; Before doing anything else, we have to setup our banking configuration.
; Otherwise just the lowest 16K are actually RAM. Writing through the ROM
; to the underlying RAM works, but it is bad style.

	lda	MMU_CR	 	; Get current memory configuration...
       	pha		 	; ...and save it for later
       	lda    	#CC65_MMU_CFG	; Bank0 with kernal ROM
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

; Initialize conio stuff

	jsr	initconio

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

; Pass an empty command line

NoIRQ1:	jsr	push0 	 	; argc
 	jsr	push0 	 	; argv

 	ldy	#4    	 	; Argument size
       	jsr    	_main 	 	; call the users code

; This is also the _exit entry. Reset the IRQ vector if we chained it.

_exit:  lda     #<__IRQFUNC_COUNT__
	beq	NoIRQ2
	lda	IRQInd+1
	ldx	IRQInd+2
	sei
	sta	IRQVec
	stx	IRQVec+1
	cli

; Run module destructors

NoIRQ2: jsr    	donelib

; Reset the conio stuff

 	jsr	doneconio

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
; that parts of our program may not be accessible. Since the crt0 module is
; the first module in the program, it will always be below $4000 and always
; in RAM. So we place several short stubs here that switch back our ROM
; config before calling our user defined handlers. These stubs are only
; used if any other code uses the interrupt or break vectors. They are dead
; code otherwise, but since there is no other way to keep them in low memory,
; they have to go here.

IRQStub:
	cld    	       		   	; Just to be sure
	lda     MMU_CR 		   	; Get old register value
	pha    	       		   	; And save on stack
	lda     #CC65_MMU_CFG	   	; Bank 0 with kernal ROM
	sta    	MMU_CR
	ldy 	#<(__IRQFUNC_COUNT__*2)
       	lda    	#<__IRQFUNC_TABLE__
	ldx 	#>__IRQFUNC_TABLE__
	jsr	condes 		   	; Call the functions
	pla    	       			; Get old register value
	sta    	MMU_CR
       	jmp    	IRQInd			; Jump to the save IRQ vector


BRKStub:
	pla    				; Get original MMU_CR value
	sta    	MMU_CR			; And set it
       	jmp    	BRKInd 	       	       	; Jump indirect to break


; ------------------------------------------------------------------------
; Data

.data
zpsave:	.res	zpspace

; Old break vector preceeded by a jump opcode
BRKOld: jmp     $0000

; Indirect vectors preceeded by a jump opcode
BRKInd: jmp     $0000

.bss
spsave:	.res	1
mmusave:.res	1



