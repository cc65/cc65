;
; Start-up code for cc65 (CX16 r35 version)
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
        stx     spsave          ; Save the system stack ptr

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

.if 0   ; (We don't need to preserve zero-page space for cc65's variables.)
; Copy back the zero-page stuff.

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2
.endif

; Restore the system stuff.

        ldx     spsave
        txs                     ; Restore stack pointer
        ldx     ramsave
        stx     VIA1::PRA       ; Restore former RAM bank
        lda     VIA1::PRB
        and     #<~$07
        ora     #$04
        sta     VIA1::PRB       ; Change back to BASIC ROM

; Back to BASIC.

        rts


; ------------------------------------------------------------------------

.segment        "ONCE"

init:
; Change from BASIC's ROM to Kernal's ROM.

        lda     VIA1::PRB
        and     #<~$07
        sta     VIA1::PRB

; Change to the second RAM bank.

        lda     VIA1::PRA
        sta     ramsave         ; Save the current RAM bank number
        lda     #$01
        sta     VIA1::PRA

.if 0   ; (We don't need to preserve zero-page space for cc65's variables.)
; Save the zero-page locations that we need.

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1
.endif

; Set up the stack.

        lda     #<(__MAIN_START__ + __MAIN_SIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

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
.if 0
zpsave: .res    zpspace
.endif
