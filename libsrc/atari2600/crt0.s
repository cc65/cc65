; Atari VCS 2600 startup code for cc65
;
; Florent Flament (contact@florentflament.com), 2017

        .export         _exit
        .export         __STARTUP__ : absolute = 1

        .import         __RAM_START__, __RAM_SIZE__
        .import         copydata
        .import         _main

        .include        "zeropage.inc"


.segment "STARTUP"
start:
; Clear decimal mode
        cld

; Initialization Loop:
; * Clears Atari 2600 whole memory (128 bytes) including BSS segment
; * Clears TIA registers
; * Sets system stack pointer to $ff (i.e top of zero-page)
        ldx     #0
        txa
clearLoop:
        dex
        txs
        pha
        bne     clearLoop

; Initialize data
        jsr     copydata

; Initialize C stack pointer
        lda     #<(__RAM_START__ + __RAM_SIZE__)
        ldx     #>(__RAM_START__ + __RAM_SIZE__)
        sta     sp
        stx     sp+1

; Call main
        jsr     _main
_exit:  jmp     _exit


.segment "VECTORS"
.word   start                   ; NMI
.word   start                   ; Reset
.word   start                   ; IRQ
