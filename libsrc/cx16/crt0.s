;
; Start-up code for cc65 (CX16 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as start-up

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         BSOUT
        .import         __MAIN_START__, __MAIN_SIZE__   ; Linker-generated
        .importzp       ST

        .include        "zeropage.inc"
        .include        "cx16.inc"


; ------------------------------------------------------------------------
; Start-up code

.segment        "STARTUP"

Start:  tsx
        stx     spsave          ; Save the system stack ptr

; Save space by putting some of the start-up code in the ONCE segment,
; which will be re-used by the BSS segment, the heap, and the C stack.

        jsr     init

; Clear the BSS data.

        jsr     zerobss

; Push the command-line arguments; and, call main().

        jsr     callmain

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
        ldx     banksave
        stx     VIA1::PRA2      ; Restore former RAM bank

; Back to BASIC.

        rts


; ------------------------------------------------------------------------

.segment        "ONCE"

init:

; Change to the first RAM bank.

        lda     VIA1::PRA2
        sta     banksave        ; Save the current bank number
        lda     #$00            ; Choose RAM bank zero
        sta     VIA1::PRA2

; Save the zero-page locations that we need.

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

; Set up the stack.

        lda     #<(__MAIN_START__ + __MAIN_SIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

; Switch to the second charset.

        lda     #$0E
        jsr     BSOUT

; Call the module constructors.

        jmp     initlib


; ------------------------------------------------------------------------
; Data

.segment        "INIT"

banksave:
        .res    1
spsave: .res    1
zpsave: .res    zpspace
