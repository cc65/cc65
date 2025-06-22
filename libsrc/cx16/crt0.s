;
; Start-up code for cc65 (CX16 r39 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as start-up

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         CHROUT
        .import         __MAIN_START__, __MAIN_SIZE__   ; Linker-generated

        .include        "zeropage.inc"
        .include        "cx16.inc"


; ------------------------------------------------------------------------
; Start-up code

.segment        "STARTUP"

Start:  tsx
        stx     spsave          ; Save the system stack ptr.

; Save space by putting some of the start-up code in the ONCE segment
; which will be re-used by the BSS segment, the heap, and the C stack.

        jsr     init

; Clear the BSS data.

        jsr     zerobss

; Push the command-line arguments, and call main().

        jsr     callmain

; Back from main() [this is also the exit() entry].

_exit:
; Put the program return code into BASIC's status variable.

        sta     STATUS

; Run the module destructors.

        jsr     donelib

; Restore the system stuff.

        ldx     spsave
        txs                     ; Restore stack pointer
        ldx     ramsave
        stx     RAM_BANK        ; Restore former RAM bank
        lda     #$04
        sta     ROM_BANK        ; Change back to BASIC ROM

; Back to BASIC.

        rts


; ------------------------------------------------------------------------

.segment        "ONCE"

init:
; Change from BASIC's ROM to Kernal's ROM.

        stz     ROM_BANK

; Change to the second RAM bank.

        lda     RAM_BANK
        sta     ramsave         ; Save the current RAM bank number
        lda     #$01
        sta     RAM_BANK

; Set up the stack.

        lda     #<(__MAIN_START__ + __MAIN_SIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__)
        sta     c_sp
        stx     c_sp+1          ; Set argument stack ptr

; Switch to the lower/UPPER PetSCII charset.

        lda     #$0E
        jsr     CHROUT

; Call the module constructors.

        jmp     initlib


; ------------------------------------------------------------------------
; Data

.segment        "INIT"

ramsave:
        .res    1
spsave: .res    1
