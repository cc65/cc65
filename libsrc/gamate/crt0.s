
        .export         Start, _exit

        .import         initlib, donelib, callmain
        .import         push0, _main, zerobss, copydata

        ; Linker generated symbols
        .import         __RAM_START__, __RAM_SIZE__

        .include        "zeropage.inc"
        .include        "gamate.inc"

Start:
        ; setup the CPU and System-IRQ

        ; Initialize CPU
        sei
        cld

        ldx     #0
        stx     ZP_IRQ_CTRL     ; disable calling cartridge IRQ/NMI handler

        ; Set up stack and memory mapping
        ;ldx     #$FF            ; Stack top ($01FF)
        dex
        txs

        ; Clear the BSS data
        jsr     zerobss

        ; Copy the .data segment to RAM
        jsr     copydata

        ; Set up the stack
        lda     #<(__RAM_START__+__RAM_SIZE__)
        ldx     #>(__RAM_START__+__RAM_SIZE__)
        sta     sp
        stx     sp + 1

        ; Call module constructors
        jsr     initlib

        lda     #1
        sta     ZP_IRQ_CTRL     ; enable calling cartridge IRQ/NMI handler
        cli                     ; allow IRQ only after constructors have run

        ; Pass an empty command line
        jsr     push0           ; argc
        jsr     push0           ; argv

        ldy     #4              ; Argument size
        jsr     _main           ; call the users code

        ; Call module destructors. This is also the _exit entry.
_exit:
        jsr     donelib         ; Run module destructors

        ; reset (start over)
        jmp     Start

        .export initmainargs
initmainargs:
        rts
