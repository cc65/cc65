;
; Startup code for cc65 (Plus/4 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
        .export         brk_jmp

	.import		condes, initlib, donelib
	.import	     	push0, _main, zerobss
	.import	       	__IRQFUNC_TABLE__, __IRQFUNC_COUNT__

        .include        "zeropage.inc"
	.include	"plus4.inc"


; ------------------------------------------------------------------------
; Constants

IRQInd 	       	= $500	; JMP $0000 - used as indirect IRQ vector

; ------------------------------------------------------------------------
; Place the startup code in a special segment to cope with the quirks of
; plus/4 banking.

.segment       	"STARTUP"

        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
        .byte   $9E,"4109"	; SYS 4109
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker

; ------------------------------------------------------------------------
; Actual code

        sei                     ; No interrupts since we're banking out the ROM
        sta     ENABLE_RAM
       	ldx   	#zpspace-1
L1:	lda	sp,x
   	sta	zpsave,x	; save the zero page locations we need
	dex
       	bpl	L1
        sta     ENABLE_ROM
        cli

; Close open files

	jsr	$FFCC           ; CLRCH

; Switch to second charset

	lda	#14
	jsr	$FFD2           ; BSOUT

; Setup the IRQ vector in the banked RAM and switch off the ROM

        sei                     ; No ints, handler not yet in place
        sta     ENABLE_RAM
        lda     #<IRQ
        sta     $FFFE           ; Install interrupt handler
        lda     #>IRQ
        sta     $FFFF
        cli                     ; Allow interrupts

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack. The stack starts at the top of the
; usable RAM.

       	tsx
       	stx    	spsave       	; save system stk ptr

        lda     #<$FD00
        sta     sp
        lda     #>$FD00
        sta     sp+1

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

; Pass an empty command line

NoIRQ1: jsr    	push0		; argc
	jsr	push0		; argv

	ldy	#4    		; Argument size
       	jsr    	_main 		; call the users code

; Back from main (this is also the _exit entry). Reset the IRQ vector if
; we chained it.

_exit:  lda     #<__IRQFUNC_COUNT__
	beq	NoIRQ2
	lda	IRQInd+1
	ldx	IRQInd+2
	sei
	sta	IRQVec
	stx	IRQVec+1
	cli

; Run module destructors.

NoIRQ2:	jsr	donelib	 	; Run module destructors

; Restore system stuff

	ldx	spsave
	txs

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Enable the ROM, reset changed vectors and return to BASIC

        sta     ENABLE_ROM
	jmp	$FF8A           ; RESTOR


; ------------------------------------------------------------------------
; IRQ handler

.segment        "LOWCODE"

IRQ:    pha
        txa
        pha
        tsx                     ; Get the stack pointer
        lda     $0103,x         ; Get the saved status register
        tax                     ; Save for later
        and     #$10            ; Test for BRK bit
        bne     dobreak
        lda     #>irq_ret       ; Push new return address
        pha
        lda     #<irq_ret
        pha
        txa
        pha
        sta     ENABLE_ROM      ; Switch to ROM
        jmp     ($FFFE)         ; Jump to kernal irq handler

irq_ret:
        sta     ENABLE_RAM      ; Switch back to RAM
        pla
        tax
        pla
        rti

dobreak:
        lda     brk_jmp+2       ; Check high byte of address
        beq     nohandler
        jmp     brk_jmp         ; Jump to the handler

; No break handler installed, jump to ROM

nohandler:
        tya
        pha                     ; ROM handler expects Y on stack
        sta     ENABLE_ROM
        jmp     (BRKVec)        ; Jump indirect to the break vector

; ------------------------------------------------------------------------
; Stub for the IRQ chain. Is used only if there are IRQs defined. Needed in
; low memory because of the banking.

.segment        "LOWCODE"

IRQStub:
        cld                     ; Just to be sure
        sta     ENABLE_RAM      ; Switch to RAM
	ldy 	#<(__IRQFUNC_COUNT__*2)
       	lda    	#<__IRQFUNC_TABLE__
	ldx 	#>__IRQFUNC_TABLE__
	jsr	condes 		   	; Call the IRQ functions
	sta    	ENABLE_ROM
       	jmp    	IRQInd			; Jump to the saved IRQ vector

; ------------------------------------------------------------------------
; Data

.data
zpsave:	        .res	zpspace

; BRK handling
brk_jmp:        jmp     $0000

.bss
spsave:	        .res	1



