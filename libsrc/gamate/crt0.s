
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib, callmain
        .import         push0, _main, zerobss, copydata

        .import         IRQStub

        ; Linker generated symbols
        .import         __RAM_START__, __RAM_SIZE__
        .import         __ROM_START__, __ROM_SIZE__
        .import         __STARTUP_LOAD__,__STARTUP_RUN__, __STARTUP_SIZE__
        .import         __CODE_LOAD__,__CODE_RUN__, __CODE_SIZE__
        .import         __RODATA_LOAD__,__RODATA_RUN__, __RODATA_SIZE__

        .include        "zeropage.inc"
        .include        "gamate.inc"

        .segment "STARTUP"

        .word 0                                         ; +00 checksum from 7000-7fff (simple 8bit adds)
        .byte 1, 0, 1                                   ; +02 flags
        .byte "COPYRIGHT BIT CORPORATION", 0, $ff       ; +05 copyright
        ; system vectors
        jmp     reset                                   ; +20 reset entry
        jmp     nmi                                     ; +23 nmi entry
        jmp     IRQStub                                 ; +26 irq entry (135 hz)

;-------------------------------------------------------------------------------
reset:
        ; setup the CPU and System-IRQ

        ; Initialize CPU
        sei
        cld

        ; Setup stack and memory mapping
        ldx     #$FF            ; Stack top ($01FF)
        txs

        ; Clear the BSS data
        jsr     zerobss

        ; Copy the .data segment to RAM
        jsr     copydata

        ; setup the stack
        lda     #<(__RAM_START__+__RAM_SIZE__)
        sta     sp
        lda     #>(__RAM_START__+__RAM_SIZE__)
        sta     sp + 1

        ; Call module constructors
        jsr     initlib

        cli     ; allow IRQ only after constructors have run

        ; Pass an empty command line
        jsr     push0           ; argc
        jsr     push0           ; argv

        ldy     #4              ; Argument size
        jsr     _main           ; call the users code

        ; Call module destructors. This is also the _exit entry.
_exit:
        jsr     donelib         ; Run module destructors

        ; reset (start over)
        jmp     reset

        .export initmainargs
initmainargs:
        rts

;-------------------------------------------------------------------------------
nmi:
        rts
