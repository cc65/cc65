;
; Startup code for cc65 (PCEngine version)
;
; by Groepaz/Hitmen <groepaz@gmx.net>
; based on code by Ullrich von Bassewitz <uz@cc65.org>
;
; This must be the *first* file on the linker command line
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         push0, _main, zerobss
        .import         initheap
        .import         IRQStub

        ; Linker generated
        .import         __RAM_START__, __RAM_SIZE__
        .import         __ROM0_START__, __ROM0_SIZE__
        .import         __ROM_START__, __ROM_SIZE__
        .import         __STARTUP_LOAD__,__STARTUP_RUN__, __STARTUP_SIZE__
        .import         __CODE_LOAD__,__CODE_RUN__, __CODE_SIZE__
        .import         __RODATA_LOAD__,__RODATA_RUN__, __RODATA_SIZE__
        .import         __DATA_LOAD__,__DATA_RUN__, __DATA_SIZE__
        .import         __BSS_SIZE__

        .include        "pce.inc"
        .include        "extzp.inc"

        .importzp       sp
        .importzp       ptr1,ptr2
        .importzp       tmp1,tmp2,tmp3

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

        .segment "STARTUP"

start:

        ; setup the CPU and System-IRQ

        ; Initialize CPU

        sei
        nop
        csh                     ; set high speed CPU mode
        nop
        cld
        nop

        ; Setup stack and memory mapping
        ldx     #$FF            ; Stack top ($21FF)
        txs

        ; at startup all MPRs are set to 0, so init them
        lda     #$ff
        tam     #%00000001      ; 0000-1FFF = Hardware page
        lda     #$F8
        tam     #%00000010      ; 2000-3FFF = Work RAM

        ; FIXME: setup a larger block of memory to use with C-code
        ;lda     #$F7
        ;tam     #%00000100      ; 4000-5FFF = Save RAM
        ;lda     #1
        ;tam     #%00001000      ; 6000-7FFF  Page 2
        ;lda     #2
        ;tam     #%00010000      ; 8000-9FFF  Page 3
        ;lda     #3
        ;tam     #%00100000      ; A000-BFFF  Page 4
        ;lda     #4
        ;tam     #%01000000      ; C000-DFFF  Page 5
        ;lda     #0
        ;tam     #%10000000      ; e000-fFFF  hucard/syscard bank 0

        ; Clear work RAM (2000-3FFF)
        stz     <$00
        tii     $2000, $2001, $1FFF

        ; Initialize hardware
        stz     TIMER_CTRL      ; Timer off
        lda     #$07
        sta     IRQ_MASK        ; Interrupts off
        stz     IRQ_STATUS      ; Acknowledge timer

        ; FIXME; i dont know why the heck this one doesnt work when called from a constructor :/
        .import vdc_init
        jsr     vdc_init

        ; Turn on background and VD interrupt/IRQ1
        lda     #$05
        sta     IRQ_MASK        ; IRQ1=on

        ; Clear the BSS data
        jsr     zerobss

        ; Copy the .data segment to RAM
        tii     __DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__

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

        ; reset the PCEngine (start over)
        jmp     start

_nmi:
        rti

        .export initmainargs
initmainargs:
        rts

; ------------------------------------------------------------------------
; hardware vectors
; ------------------------------------------------------------------------
        .segment "VECTORS"

        .word   IRQStub         ; $fff6 IRQ2 (External IRQ, BRK)
        .word   IRQStub         ; $fff8 IRQ1 (VDC)
        .word   IRQStub         ; $fffa Timer
        .word   _nmi            ; $fffc NMI
        .word   start           ; $fffe reset
