;
; Startup code for cc65 (CreatiVision version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .export         irq2
        .import         zerobss, copydata
        .import         initlib, donelib, callmain
        .import         __VECTORS_LOAD__, __VECTORS_RUN__, __VECTORS_SIZE__
        .import         __ZP_LAST__, __STACKSIZE__, __RAM_START__

        .include        "creativision.inc"
        .include        "zeropage.inc"

; ------------------------------------------------------------------------

entry:
        ; Init the CPU
        sei
        cld

        ; Copy the IRQ vectors
        ldx     #<__VECTORS_SIZE__ - 1
:       lda     __VECTORS_LOAD__,x
        sta     __VECTORS_RUN__,x
        dex
        bpl     :-

        ; Setup the CPU stack ptr
        ldx     #<__RAM_START__ - 1
        txs

        ; Clear the BSS data
        jsr     zerobss

        ; Copy data from ROM to RAM
        jsr     copydata

        ; Setup the argument stack ptr
        lda     #<(__ZP_LAST__ + __STACKSIZE__)
        ldx     #>(__ZP_LAST__ + __STACKSIZE__)
        sta     sp
        stx     sp+1

        ; Call module constructors
        jsr     initlib

        ; enable vertical blank interrupts in the display controller
        lda      #$E0           ; 16K RAM, Active Display, Mode 1, VBI enabled
        ldx      #$01           ; Register 1
        jsr      BIOS_WRITE_VDP_REG

        ; Start interrupts
        cli

        ; Call main()
        jsr     callmain

        ; Call module destructors. This is also the _exit entry.
_exit:  jsr     donelib

        ; A Creativision program isn't supposed to exit.
loop:   jmp loop

; ------------------------------------------------------------------------
; Define the IRQ vectors.

.segment        "VECTORS"

irq1:   jmp     BIOS_IRQ1_ADDR
irq2:   jmp     BIOS_IRQ2_ADDR

; ------------------------------------------------------------------------
; Define CART setup values for BIOS.

.segment        "SETUP"

        ; BIOS Jump Start
        ; This is where the entry point of the program needs to be
        .addr   entry
        .addr   irq2

        .res    4

        ; VDP Setup
        ; This sets to Graphics Mode 1
        .byte   $00             ; Register 0
        .byte   $C0             ; Register 1 16K RAM, Active Display, Mode 1, VBI disabled
        .byte   $04             ; Register 2 Name Table at $1000 - $12FF
        .byte   $60             ; Register 3 Colour Table at $1800 - $181F
        .byte   $00             ; Register 4 Pattern Table at $0000 - $07FF
        .byte   $10             ; Register 5 Sprite Attribute at $0800 - $087F
        .byte   $01             ; Register 6 Sprite Pattern
        .byte   $F1             ; Register 7 Text colour Foreground / background

        .res    4

        ; BIOS Vector after NMI or RESET
        ; Keeping with retail cartridges, we jump back to BIOS ROM and have it
        ; setup zeropage etc, and show the Creativision logo and copyright.
        .addr BIOS_NMI_RESET_ADDR

        ; BIOS Short Interrupt Handler
        ; Vectored from BIOS ROM:FE2C. This should contain a pointer to the user's
        ; BIOS interrupt handler.
        .addr irq1

; ------------------------------------------------------------------------
