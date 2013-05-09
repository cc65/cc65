;
; Startup code for cc65 (C64 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         initlib, donelib
        .import         zerobss
        .import         callmain
        .import         RESTOR, BSOUT, CLRCH
        .import         __RAM_START__, __RAM_SIZE__     ; Linker generated
        .import         __STACKSIZE__                   ; Linker generated
        .importzp       ST

        .include        "zeropage.inc"
        .include        "c64.inc"


; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

Start:

; Save the zero page locations we need

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

; Switch to second charset

        lda     #14
        jsr     BSOUT

; Switch off the BASIC ROM

        lda     $01
        pha                     ; Remember the value
        and     #$F8
        ora     #$06            ; Enable kernal+I/O, disable basic
        sta     $01

; Clear the BSS data

        jsr     zerobss

; Save system settings and setup the stack

        pla
        sta     mmusave         ; Save the memory configuration

        tsx
        stx     spsave          ; Save the system stack ptr

        lda     #<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp
        lda     #>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp+1            ; Set argument stack ptr

; Call module constructors

        jsr     initlib

; Push arguments and call main

        jsr     callmain

; Back from main (This is also the _exit entry). Run module destructors

_exit:  pha                     ; Save the return code on stack
        jsr     donelib

; Copy back the zero page stuff

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Place the program return code into ST

        pla
        sta     ST

; Restore system stuff

        ldx     spsave
        txs                     ; Restore stack pointer
        ldx     mmusave
        stx     $01             ; Restore memory configuration

; Back to basic

        rts

; ------------------------------------------------------------------------
; Data

.segment        "ZPSAVE"

zpsave: .res    zpspace

.bss

spsave: .res    1
mmusave:.res    1
