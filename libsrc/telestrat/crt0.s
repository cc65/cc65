;
; Startup code for cc65 (Oric version)
;
; By Debrune Jérôme <jede@oric.org> and Ullrich von Bassewitz <uz@cc65.org>
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         callmain, zerobss
        .import         __MAIN_START__, __MAIN_SIZE__

        .include        "zeropage.inc"
        .include        "telestrat.inc"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment        "STARTUP"

        tsx
        stx     spsave          ; Save system stk ptr

; Save space by putting some of the start-up code in a segment
; that will be re-used.

        jsr     init

; Clear the BSS variables (after the constructors have been run).

        jsr     zerobss

; Push the command-line arguments; and, call main().

        jsr     callmain

; Call the module destructors. This is also the exit() entry.

_exit:  jsr     donelib

; Restore the system stuff.

        ldx     spsave
        txs

; Copy back the zero-page stuff.

        ldx     #zpspace - 1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Back to BASIC.

        rts

; ------------------------------------------------------------------------
; Put this code in a place that will be re-used by BSS, the heap,
; and the C stack.

.segment        "ONCE"

; Save the zero-page area that we're about to use.

init:   ldx     #zpspace - 1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1


; Set up the C stack.

        lda     #<(__MAIN_START__ + __MAIN_SIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

; Call the module constructors.

        jmp     initlib

; ------------------------------------------------------------------------

.segment        "INIT"

spsave: .res    1
stsave: .res    1
zpsave: .res    zpspace
