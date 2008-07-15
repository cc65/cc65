;
; Startup code for cc65 (Apple2 version)
;
; This must be the *first* file on the linker command line
;

        .export         _exit
        .import         zerobss
        .import    	initlib, donelib
        .import    	callmain, callirq
        .import        	__RAM_START__ , __RAM_LAST__	; Linker generated
        .import         __MOVE_START__, __MOVE_LAST__	; Linker generated
        .import         __LC_START__  , __LC_LAST__	; Linker generated
        .import        	__ZPSAVE_RUN__, __INIT_SIZE__	; Linker generated
        .import        	__INTERRUPTOR_COUNT__		; Linker generated

        .include        "zeropage.inc"
        .include        "apple2.inc"

        .linecont	+

; ------------------------------------------------------------------------

        .segment        "EXEHDR"

        .addr           __RAM_START__			; Start address
        .word           __ZPSAVE_RUN__ - __RAM_START__ + \
			__MOVE_LAST__  - __MOVE_START__	; Size

; ------------------------------------------------------------------------

        .segment        "STARTUP"

        ; ProDOS TechRefMan, chapter 5.2.1:
        ; "For maximum interrupt efficiency, a system program should not
        ;  use more than the upper 3/4 of the stack."
        ldx     #$FF
        txs            		; Init stack pointer

	; Switch in LC bank 2 for W/O
	bit	$C081
	bit	$C081
	
	; Set source start address
	lda	#<(__ZPSAVE_RUN__ + __INIT_SIZE__)
	ldy	#>(__ZPSAVE_RUN__ + __INIT_SIZE__)
	sta	$9B
	sty	$9C
	
	; Set source last address
	lda	#<(__ZPSAVE_RUN__ + __INIT_SIZE__ + __LC_LAST__ - __LC_START__)
	ldy	#>(__ZPSAVE_RUN__ + __INIT_SIZE__ + __LC_LAST__ - __LC_START__)
	sta	$96
	sty	$97

	; Set destination last address
	lda	#<__LC_LAST__
	ldy	#>__LC_LAST__
	sta	$94
	sty	$95

	; Call into Applesoft Block Transfer Utility - which handles zero
	; sized blocks well - to move content of the LC memory area
	jsr	$D396		; BLTU + 3

	; Set source start address
	lda	#<__ZPSAVE_RUN__
	ldy	#>__ZPSAVE_RUN__
	sta	$9B
	sty	$9C
	
	; Set source last address
	lda	#<(__ZPSAVE_RUN__ + __INIT_SIZE__)
	ldy	#>(__ZPSAVE_RUN__ + __INIT_SIZE__)
	sta	$96
	sty	$97

	; Set destination last address
	lda	#<__RAM_LAST__
	ldy	#>__RAM_LAST__
	sta	$94
	sty	$95

	; Call into Applesoft Block Transfer Utility - which handles moving
	; overlapping blocks upwards well - to move the INIT segment
	jsr	$D396		; BLTU + 3

        ; Delegate all further processing to keep the STARTUP segment small
        jsr     init

        ; Avoid re-entrance of donelib. This is also the _exit entry
_exit:  ldx     #<exit
        lda     #>exit
        jsr     reset		; Setup RESET vector
        
        ; Switch in ROM in case it wasn't already switched in by a RESET
	bit	$C082

        ; Call module destructors
        jsr     donelib

        ; Check for valid interrupt vector table entry number
        lda     intnum
        beq     exit

        ; Deallocate interrupt vector table entry
        dec     params		; Adjust parameter count
        jsr     $BF00		; MLI call entry point
        .byte   $41		; Dealloc interrupt
        .addr   params

        ; Restore the original RESET vector
exit:   ldx     #$02
:       lda     rvsave,x
        sta     SOFTEV,x
        dex
        bpl     :-

        ; Copy back the zero page stuff
        ldx     #zpspace-1
:       lda     zpsave,x
        sta     sp,x
        dex
        bpl     :-

        ; ProDOS TechRefMan, chapter 5.2.1:
        ; "System programs should set the stack pointer to $FF at the
        ;  warm-start entry point."
        ldx     #$FF
        txs                     ; Re-init stack pointer

        ; Back to DOS
        jmp     DOSWARM

; ------------------------------------------------------------------------

        .segment        "INIT"

        ; Save the zero page locations we need
init:   ldx     #zpspace-1
:       lda     sp,x
        sta     zpsave,x
        dex
        bpl     :-

        ; Clear the BSS data
        jsr     zerobss

        ; Save the original RESET vector
        ldx     #$02
:       lda     SOFTEV,x
        sta     rvsave,x
        dex
        bpl     :-

        ; ProDOS TechRefMan, chapter 5.3.5:
        ; "Your system program should place in the RESET vector the
        ;  address of a routine that ... closes the files."
        ldx     #<_exit
        lda     #>_exit
        jsr     reset		; Setup RESET vector
                
        ; Setup the stack
        lda     HIMEM
        sta     sp
        lda     HIMEM+1
        sta     sp+1   	    	; Set argument stack ptr

        ; Check for interruptors
        lda     #<__INTERRUPTOR_COUNT__
        beq     :+

        ; Check for ProDOS
        lda     $BF00		; MLI call entry point
        cmp     #$4C		; Is MLI present? (JMP opcode)
        bne     prterr

        ; Allocate interrupt vector table entry
        jsr     $BF00		; MLI call entry point
        .byte   $40		; Alloc interrupt
        .addr   params
        bcs     prterr

	; Enable interrupts as old ProDOS versions (i.e. 1.1.1)
	; jump to SYS and BIN programs with interrupts disabled
	cli

        ; Call module constructors
:       jsr     initlib

	; Switch in LC bank 2 for R/O
	bit	$C080

        ; Push arguments and call main()
        jmp     callmain

        ; Print error message and return
prterr: ldx     #msglen-1
:       lda     errmsg,x
        jsr     $FDED		; COUT
        dex
        bpl     :-
        rts

errmsg: .ifdef  __APPLE2ENH__
        .byte   $8D,     't'|$80, 'p'|$80, 'u'|$80, 'r'|$80, 'r'|$80
        .byte   'e'|$80, 't'|$80, 'n'|$80, 'i'|$80, ' '|$80, 'c'|$80
        .byte   'o'|$80, 'l'|$80, 'l'|$80, 'a'|$80, ' '|$80, 'o'|$80
        .byte   't'|$80, ' '|$80, 'd'|$80, 'e'|$80, 'l'|$80, 'i'|$80
        .byte   'a'|$80, 'F'|$80, $8D
        .else
        .byte   $8D,     'T'|$80, 'P'|$80, 'U'|$80, 'R'|$80, 'R'|$80
        .byte   'E'|$80, 'T'|$80, 'N'|$80, 'I'|$80, ' '|$80, 'C'|$80
        .byte   'O'|$80, 'L'|$80, 'L'|$80, 'A'|$80, ' '|$80, 'O'|$80
        .byte   'T'|$80, ' '|$80, 'D'|$80, 'E'|$80, 'L'|$80, 'I'|$80
        .byte   'A'|$80, 'F'|$80, $8D
        .endif

msglen = * - errmsg

; ------------------------------------------------------------------------

        .segment        "LOWCODE"

        ; ProDOS TechRefMan, chapter 6.2:
        ; "Each installed routine must begin with a CLD instruction."
intrpt: cld

        ; Call interruptors and check for success
        jsr     callirq
        bcc	:+

        ; ProDOS TechRefMan, chapter 6.2:
        ; "When the routine that can process the interrupt is called, it
        ;  should ... return (via an RTS) with the carry flag clear."
        clc
        rts

        ; ProDOS TechRefMan, chapter 6.2:
        ; "When a routine that cannot process the interrupt is called,
        ;  it should return (via an RTS) with the cary flag set ..."
:       sec
        rts

        ; Setup RESET vector
reset:  stx     SOFTEV
        sta     SOFTEV+1
        eor     #$A5
        sta     PWREDUP
        rts

; ------------------------------------------------------------------------

        .data

params: .byte   $02		; Parameter count
intnum: .byte   $00		; Interrupt number
        .addr   intrpt		; Interrupt handler

; ------------------------------------------------------------------------

        .segment        "ZPSAVE"

zpsave: .res    zpspace

; ------------------------------------------------------------------------

        .bss

rvsave: .res    3
