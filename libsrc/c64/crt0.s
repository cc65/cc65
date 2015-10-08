;
; Startup code for cc65 (C64 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         moveinit, zerobss, callmain
        .import         BSOUT
        .import         __RAM_START__, __RAM_SIZE__     ; Linker generated
        .import         __STACKSIZE__                   ; from configure file
        .importzp       ST

        .include        "zeropage.inc"
        .include        "c64.inc"


; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

Start:

; Switch to the second charset.

        lda     #14
        jsr     BSOUT

; Switch off the BASIC ROM.

        lda     $01
        sta     mmusave         ; Save the memory configuration
        and     #$F8
        ora     #$06            ; Enable Kernal+I/O, disable BASIC
        sta     $01

        tsx
        stx     spsave          ; Save the system stack ptr

; Allow some re-entrancy by skipping the next task if it already was done.
; This often can let us rerun the program without reloading it.

        ldx     move_init
        beq     L0

; Move the INIT segment from where it was loaded (over ZPSAVE and BSS)
; into where it must be run (in the heap).

        jsr     moveinit
        dec     move_init       ; set to false

; Save space by putting the rest of the start-up code in the INIT segment,
; which can be re-used by the heap and the C stack.

L0:     jsr     initstart

; Back from main() [this is also the exit() entry]. Run the module destructors.

_exit:  pha                     ; Save the return code on stack
        jsr     donelib

; Copy back the zero-page stuff.

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Place the program return code into BASIC's status variable.

        pla
        sta     ST

; Restore the system stuff.

        ldx     spsave
        txs                     ; Restore stack pointer
        ldx     mmusave
        stx     $01             ; Restore memory configuration

; Back to BASIC.

        rts


; ------------------------------------------------------------------------

.segment        "INIT"

initstart:

; Save the zero-page locations that we need.

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

; Clear the BSS data.

        jsr     zerobss

; Set up the stack.

        lda     #<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        ldx     #>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

; Call the module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jmp     callmain


; ------------------------------------------------------------------------
; Data

.data

mmusave:.res    1
spsave: .res    1
move_init:
        .byte   1

.segment        "ZPSAVE"

zpsave: .res    zpspace
