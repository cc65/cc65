;
; Startup code for cc65 (Plus/4 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		initlib, donelib
	.import	     	push0, _main, zerobss
	.import	       	__IRQFUNC_TABLE__, __IRQFUNC_COUNT__

        .include        "zeropage.inc"
	.include	"plus4.inc"


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
        tax
        lda     #>irqret        ; Push new return address
        pha
        lda     #<irqret
        pha
        txa
        pha
        sta     ENABLE_ROM      ; Switch to ROM
        jmp     ($FFFE)         ; Jump to kernal irq handler

irqret: sta     ENABLE_RAM      ; Switch back to RAM
        pla
        tax
        pla
        rti

; ------------------------------------------------------------------------
; Data

.data
zpsave:	.res	zpspace

.bss
spsave:	.res	1


