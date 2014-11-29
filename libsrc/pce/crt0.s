;
; Startup code for cc65 (PCEngine version)
;
; by Groepaz/Hitmen <groepaz@gmx.net>
; based on code by Ullrich von Bassewitz <uz@cc65.org>
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		initlib, donelib
	.import	    push0, _main, zerobss
	.import     initheap
	.import		tmp1,tmp2,tmp3

	.import		__RAM_START__, __RAM_SIZE__	; Linker generated
;;	.import		__SRAM_START__, __SRAM_SIZE__	; Linker generated
	.import		__ROM0_START__, __ROM0_SIZE__	; Linker generated
	.import		__ROM_START__, __ROM_SIZE__	; Linker generated
	.import		__STARTUP_LOAD__,__STARTUP_RUN__, __STARTUP_SIZE__	; Linker generated
	.import		__CODE_LOAD__,__CODE_RUN__, __CODE_SIZE__	; Linker generated
	.import		__RODATA_LOAD__,__RODATA_RUN__, __RODATA_SIZE__	; Linker generated
	.import		__DATA_LOAD__,__DATA_RUN__, __DATA_SIZE__	; Linker generated
	.import __BSS_SIZE__

	.include "pcengine.inc"

    .importzp       sp
	.importzp	ptr1,ptr2

; ------------------------------------------------------------------------
; Create an empty LOWCODE segment to avoid linker warnings

.segment        "LOWCODE"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

start:

; setup the CPU and System-IRQ

                        ; Initialize CPU

                        sei
                        nop
                        csh
                        nop
                        cld
                        nop

                        ; Setup stack and memory mapping
                        ldx     #$FF    ; Stack top ($21FF)
                        txs
                        txa
                        tam     #0      ; 0000-1FFF = Hardware page

                        lda     #$F8
                        tam     #1      ; 2000-3FFF = Work RAM

                        lda     #$F7
                        tam     #2      ; 4000-5FFF = Save RAM

						lda 	#1
                        tam     #3      ; 6000-7FFF  Page 2
						lda 	#2
                        tam     #4      ; 8000-9FFF  Page 3
						lda 	#3
                        tam     #5      ; A000-BFFF  Page 4
						lda 	#4
                        tam     #6      ; C000-DFFF  Page 5

                        ; Initialize hardware
                        stz     TIMER_COUNT   ; Timer off
                        lda     #$07
                        sta     IRQ_MASK     ; Interrupts off
                        stz     IRQ_STATUS   ; Acknowledge timer

                        ; Clear work RAM
                        stz     <$00
                        tii     $2000, $2001, $1FFF

						;; i dont know why the heck this one doesnt
						;; work when called from a constructor :/
						.import vdc_init
						jsr     vdc_init
;;                        jsr     joy_init

                        ; Turn on background and VD interrupt/IRQ1
                        lda     #$05
                        sta     IRQ_MASK           ; IRQ1=on
                        cli

; Clear the BSS data

		jsr	zerobss

; Copy the .data segment to RAM

		lda #<(__DATA_LOAD__)
		;;lda #<(__ROM0_START__ + __STARTUP_SIZE__+ __CODE_SIZE__+ __RODATA_SIZE__)
		;;lda #<(__ROM_START__ + __CODE_SIZE__+ __RODATA_SIZE__)
		sta ptr1
		lda #>(__DATA_LOAD__)
		;;lda #>(__ROM_START__ + __CODE_SIZE__+ __RODATA_SIZE__)
		sta ptr1+1
		lda #<(__DATA_RUN__)
		;;lda #<(__SRAM_START__)
		sta ptr2
		lda #>(__DATA_RUN__)
		;;lda #>(__SRAM_START__)
		sta ptr2+1

		ldx #>(__DATA_SIZE__)

@l2:
		beq @s1	; no more full pages

		; copy one page
		ldy #0
@l1:
		lda (ptr1),y
		sta (ptr2),y
		iny
		bne @l1

		inc ptr1+1
		inc ptr2+1

		dex
		bne @l2

		; copy remaining bytes
@s1:

		; copy one page
		ldy #0
@l3:
		lda (ptr1),y
		sta (ptr2),y
		iny
		cpy #<(__DATA_SIZE__)
		bne @l3

; setup the stack

;		lda #<(__RAM_START__ + __DATA_SIZE__ + __BSS_SIZE__)
	lda #<(__RAM_START__+__RAM_SIZE__)
		sta	sp
;		lda	#>(__RAM_START__ + __DATA_SIZE__ + __BSS_SIZE__)
	lda #>(__RAM_START__+__RAM_SIZE__)
       	sta	sp+1  		; Set argument stack ptr

; Init the Heap
		jsr initheap

;jmp *

; Call module constructors

  		jsr	initlib
;		.import initconio
;		jsr initconio
; Pass an empty command line


;jmp *

		jsr push0		; argc
		jsr	push0		; argv
go:
		ldy	#4    		; Argument size
       	jsr    	_main 	; call the users code

; Call module destructors. This is also the _exit entry.

_exit:
		jsr	donelib		; Run module destructors

; reset the PCEngine

		jmp start

; ------------------------------------------------------------------------
; System V-Blank Interupt
; ------------------------------------------------------------------------

_irq1:
                        pha
                        phx
                        phy


		inc _tickcount
		bne @s
		inc _tickcount+1
@s:

                        ; Acknowlege interrupt
						ldaio VDC_CTRL

						ply
                        plx
                        pla
                        rti
_irq2:
                        rti
_nmi:
                        rti
_timer:
                        stz     IRQ_STATUS
                        rti

	.export initmainargs
initmainargs:
	rts

; ------------------------------------------------------------------------
; hardware vectors
; ------------------------------------------------------------------------
    .segment "VECTORS"
    ;;.org    $fff6

    .word   _irq2        ; $fff6 IRQ2 (External IRQ, BRK)
    .word   _irq1        ; $fff8 IRQ1 (VDC)
    .word   _timer       ; $fffa Timer
    .word   _nmi         ; $fffc NMI
    .word   start		 ; $fffe reset



