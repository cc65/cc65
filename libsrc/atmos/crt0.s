;
; Startup code for cc65 (Oric version)
;
; By Debrune Jérôme <jede@oric.org> and Ullrich von Bassewitz <uz@cc65.org>
; 2015-01-09, Greg King
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         initlib, donelib
        .import         callmain, zerobss
        .import         __MAIN_START__, __MAIN_SIZE__, __STACKSIZE__

        .include        "zeropage.inc"
        .include        "atmos.inc"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment        "STARTUP"

; Save the zero-page area that we're about to use.

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

; Clear the BSS data.

        jsr     zerobss

; Currently, color isn't supported on the text screen.
; Unprotect screen columns 0 and 1 (where each line's color codes would sit).

        lda     STATUS
        sta     stsave
        and     #%11011111
        sta     STATUS

; Save some system stuff; and, set up the stack.

        tsx
        stx     spsave          ; Save system stk ptr

        lda     #<(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

; Call the module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jsr     callmain

; Call the module destructors. This is also the exit() entry.

_exit:  jsr     donelib

; Restore the system stuff.

        ldx     spsave
        txs
        lda     stsave
        sta     STATUS

; Copy back the zero-page stuff.

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Back to BASIC.

        rts

; ------------------------------------------------------------------------

.segment        "ZPSAVE1"

zpsave:

; This padding is needed by a bug in the ROM.
; (The CLOAD command starts BASIC's variables table on top of the last byte
; that was loaded [instead of at the next address].)
; This is overlaid on a buffer, so that it doesn't use extra space in RAM.

        .byte   0

; The segments "ZPSAVE1" and "ZPSAVE2" always must be together.
; They create a single object (the zpsave buffer).

.segment        "ZPSAVE2"

        .res    zpspace - 1

; ------------------------------------------------------------------------

.bss

spsave: .res    1
stsave: .res    1
