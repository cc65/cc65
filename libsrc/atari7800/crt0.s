        .export     _zonecounter
        .export     __STARTUP__ : absolute = 1
        .export     _exit
        .import     __ROM_START__
        .import     __RAM3_START__, __RAM3_SIZE__
        .import     initlib, donelib
        .import     zerobss, copydata
        .import     IRQStub
        .import     push0, _main
        .include    "atari7800.inc"
        .include    "zeropage.inc"

INPTCTRL        =       $01

        .segment "STARTUP"
start:
        ; Startup sequence recommended by Atari.
        ; See the 7800 standards document.
        sei                     ; Initialize 6502
        cld
        lda     #$07            ; Lock machine in 7800 mode
        sta     INPTCTRL
        lda     #$7f            ; DMA off
        sta     CTRL
        ldx     #0              ; OFFSET must always be 0
        stx     OFFSET
        stx     INPTCTRL        ; Make sure joysticks don't freeze
        dex                     ; Stack pointer = $ff
        txs

        ; Set up parameter stack
        lda     #<(__RAM3_START__ + __RAM3_SIZE__)
        sta     sp
        lda     #>(__RAM3_START__ + __RAM3_SIZE__)
        sta     sp+1

        jsr     copydata
        jsr     zerobss
        jsr     initlib

        ; Call main program (pass empty command line)
        jsr     push0           ; argc
        jsr     push0           ; argv
        ldy     #4              ; Argument size
        jsr     _main

_exit:
        jsr     donelib
        jmp     start

NMIHandler:
        inc     _zonecounter
        jmp     IRQStub

IRQHandler:
        rti

        .segment "DATA"
_zonecounter:
        .byte   0

        .segment "ENCRYPTION"
        .res    126, $ff        ; Reserved for encryption
Lfff8:  .byte   $ff             ; Region verification (always $ff)
Lfff9:  .byte   $f7             ; Use last 4096 bytes only for encryption
;;;Lfff9:  .byte   <(((__ROM_START__/4096)<<4) | 7)

        .segment "VECTORS"
        .word   NMIHandler
        .word   start
        .word   IRQHandler
