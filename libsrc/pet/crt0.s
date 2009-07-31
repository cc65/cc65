;
; Startup code for cc65 (PET version)
;

	.export	      	_exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
	.import		initlib, donelib, callirq
       	.import	       	zerobss, push0
	.import		callmain
        .import         CLRCH, BSOUT
	.import		__INTERRUPTOR_COUNT__

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

; Switch to second charset. The routine that is called by BSOUT to switch the
; character set will use FNLEN as temporary storage - YUCK! Since the
; initmainargs routine, which parses the command line for arguments needs this
; information, we need to save and restore it here.
; Thanks to Stefan Haubenthal for this information!

        lda     FNLEN
        pha                     ; Save FNLEN
	lda	#14
;	sta	$E84C		; See PET FAQ
	jsr	BSOUT
        pla
        sta     FNLEN           ; Restore FNLEN

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

       	tsx
       	stx    	spsave 		; Save the system stack ptr

	lda    	MEMSIZE
	sta	sp
	lda	MEMSIZE+1
       	sta	sp+1   	 	; Set argument stack ptr

; If we have IRQ functions, chain our stub into the IRQ vector

	lda	#<__INTERRUPTOR_COUNT__
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

        jsr	callmain

; Call module destructors. This is also the _exit entry.

_exit:  pha			; Save the return code on stack
        jsr	donelib

; Reset the IRQ vector if we chained it.

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

; Store the program return code into ST

	pla
	sta	ST

; Restore the stack pointer

	ldx	spsave
	txs	    	 	; Restore stack pointer

; Back to basic

       	rts

; ------------------------------------------------------------------------
; The IRQ vector jumps here, if condes routines are defined with type 2.

IRQStub:
	cld    				; Just to be sure
	jsr    	callirq                 ; Call the functions
	jmp    	IRQInd			; Jump to the saved IRQ vector

; ------------------------------------------------------------------------
; Data

.data

IRQInd: jmp	$0000

.segment        "ZPSAVE"

zpsave:	.res	zpspace

.bss

spsave:	.res	1
mmusave:.res	1

