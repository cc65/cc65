;
; Start-up code for cc65 (PC-Engine version)
;
; by Groepaz/Hitmen <groepaz@gmx.net>,
; based on code by Ullrich von Bassewitz <uz@cc65.org>
;
; 2018-02-24, Greg King
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as start-up

        .import         initlib, donelib
        .import         push0, _main
        .import         IRQStub, __nmi
        .importzp       sp

        ; Linker-generated
        .import         __CARTSIZE__
        .import         __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__
        .import         __BSS_RUN__, __BSS_SIZE__
        .import         __MAIN_START__, __MAIN_SIZE__, __STACKSIZE__

        .include        "pce.inc"
        .include        "extzp.inc"

; ------------------------------------------------------------------------
; Place the start-up code in a special segment.

.segment        "STARTUP"

        ; Initialize the CPU.
start:  sei
        nop
        csh                     ; Set high-speed CPU mode
        nop

        ; Set up the stack and the memory mapping.
        ldx     #$FF            ; Stack top ($21FF)
        txs

        ; At power-on, most MPRs have random values; so, initiate them.
        lda     #$FF
        tam     #%00000001      ; $0000-$1FFF = Hardware bank
        lda     #$F8
        tam     #%00000010      ; $2000-$3FFF = Work RAM
        ;lda     #$F7
        ;tam     #%00000100      ; $4000-$47FF = 2K Battery-backed RAM
        ;lda     #4
        ;tam     #%00001000      ; $6000-$7FFF

        lda     #$01
        ldx     #>$8000
        cpx     #>__CARTSIZE__
        bcc     @L1             ;(blt)
        tam     #%00010000      ; $8000-$9FFF = ROM bank 1 (32K block of ROM)
        inc     a
        tam     #%00100000      ; $A000-$BFFF = ROM bank 2
        inc     a
@L1:    tam     #%01000000      ; $C000-$DFFF = ROM bank 3 (32K) or 1 (16K)
        ;lda    #$00            ; (The reset default)
        ;tam    #%10000000      ; $E000-$FFFF  Hucard/Syscard bank 0

        ; Initialize the hardware.
        stz     TIMER_CTRL      ; Timer off
        lda     #%00000111
        sta     IRQ_MASK        ; Interrupts off

        ; FIXME; I don't know why the heck this one doesn't work when called from a constructor. -Groepaz :-/
.if 0   ; It now seems to work (at least, in Mednafen). -Greg King
        .import vdc_init
        jsr     vdc_init
.endif

        ; Allow interrupts from the VDC.
        lda     #%00000101
        sta     IRQ_MASK        ; IRQ1 = on

        ; Copy the .data segment to RAM
        tii     __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__

        ; Clear the .bss segment
        stz     __BSS_RUN__
        tii     __BSS_RUN__, __BSS_RUN__ + 1, __BSS_SIZE__ - 1

        ; Set up the stack
        lda     #<(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        sta     sp
        stx     sp+1

        ; Call the module constructors.
        jsr     initlib

        stz     IRQ_STATUS      ; Clear IRQs
        cli                     ; Allow IRQ only after constructors have run

        ; Pass an empty command line
        jsr     push0           ; argc
        jsr     push0           ; argv

        ldy     #4              ; Argument size
        jsr     _main           ; Call the user's code

        ; Call the module destructors. This is also the exit() entry.
_exit:  jsr     donelib

        ; Reset the PCEngine (start over).
        jmp     start

        .export initmainargs
initmainargs:
        rts

; ------------------------------------------------------------------------
; hardware vectors
; ------------------------------------------------------------------------
.segment        "VECTORS"

        .word   IRQStub         ; $FFF6 IRQ2 (External IRQ, BRK)
        .word   IRQStub         ; $FFF8 IRQ1 (VDC)
        .word   IRQStub         ; $FFFA Timer
        .word   __nmi           ; $FFFC NMI
        .word   start           ; $FFFE reset
