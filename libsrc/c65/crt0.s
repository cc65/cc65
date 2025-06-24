;
; Startup code for cc65 (C65 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         BSOUT
        .import         __MAIN_START__, __MAIN_SIZE__   ; Linker generated
        .import         __STACKSIZE__                   ; from configure file

        .include        "zeropage.inc"
        .include        "c65.inc"


; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

Start:

; Switch off the BASIC ROM.

; FIXME
;        lda     $01
;        sta     mmusave         ; Save the memory configuration
;        and     #$F8
;        ora     #$06            ; Enable Kernal+I/O, disable BASIC
;        sta     $01
;        sei
;        lda #%00000000  ; lower offset 15-8
;        ldx #%00000000  ; map blk3-1 | lower offset 19-6
;        ldy #%00000000  ; upper offset 15-8
;        ldz #%00000000  ; map blk7-4 | upper offset 19-6
;        map
;        eom

        tsx
        stx     spsave          ; Save the system stack ptr

; Save space by putting some of the start-up code in the ONCE segment,
; which can be re-used by the BSS segment, the heap and the C stack.

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
        sta     c_sp,x
        dex
        bpl     L2

; Place the program return code into BASIC's status variable.

        pla
        sta     STATUS

; Restore the system stuff.

        ldx     spsave
        txs                     ; Restore stack pointer

; Back to BASIC.

        rts


; ------------------------------------------------------------------------

.segment        "ONCE"

init:

; Save the zero-page locations that we need.

        ldx     #zpspace-1
L1:     lda     c_sp,x
        sta     zpsave,x
        dex
        bpl     L1

; Set up the stack.

        lda     #<(__MAIN_START__ + __MAIN_SIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__)
        sta     c_sp
        stx     c_sp+1          ; Set argument stack ptr

; Switch to the second charset.

; FIXME
        lda     #14
        jsr     BSOUT

; Call the module constructors.

        jmp     initlib


; ------------------------------------------------------------------------
; Data

.segment        "INIT"

mmusave:.res    1
spsave: .res    1
zpsave: .res    zpspace
