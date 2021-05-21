;
; Startup code for cc65 (C64 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         BSOUT
        .import         __MAIN_START__, __MAIN_SIZE__   ; Linker generated
        .import         __STACKSIZE__                   ; from configure file
        .importzp       ST

        .include        "zeropage.inc"


; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

Start:

; Switch off the BASIC ROM.

        lda     $01
        sta     mmusave         ; Save the memory configuration
        and     #$F8
        ora     #$06            ; Enable Kernal+I/O, disable BASIC
        sta     $01

        tsx
        stx     spsave          ; Save the system stack ptr

; Save space by putting some of the start-up code in the ONCE segment,
; which can be re-used by the BSS segment, the heap and the C stack.

        jsr     init

; Clear the BSS data.

        jsr     zerobss

; Push the command-line arguments; and, call main().

        jsr     callmain

        ; Avoid a re-entrance of donelib. This is also the exit() entry.
_exit:
        sta     stsave            ; Save the return code

        ldx     #<exit
        lda     #>exit
        jsr     setnmi

; Run the module destructors.

        jsr     donelib

; Restore the original NMI vector.
exit:   ldx     #$01
:       lda     nmisave,x
        sta     $0318,x
        dex
        bpl     :-

; Copy back the zero-page stuff.
        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Place the program return code into BASIC's status variable.

        lda     stsave
        sta     ST

; Restore the system stuff.

        ldx     spsave
        txs                     ; Restore stack pointer
        ldx     mmusave
        stx     $01             ; Restore memory configuration

; Back to BASIC.

        rts

.segment        "CODE"
setnmi:
        stx     $0318
        sta     $0319
        rts

exitnmi:
        pha
        ; save $01
        lda     $01
        pha
        ; enable i/o (switch to standard memory config)
        lda     #$36
        sta     $01
        ; check run/stop
        lda     #$7f
        sta     $dc00
        lda     $dc01
        bpl     :+              ; branch if pressed
        ; restore $01
        pla
        sta     $01
        pla
        ; continue with saved system vector
        jmp     (nmisave)
:
        ; throw away saved $01 value and akku (keep memory config)
        pla
        pla
        lda     #$ff    ; exit code (-1)
        jmp     _exit

; ------------------------------------------------------------------------

.segment        "ONCE"

init:

; Save the zero-page locations that we need.

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

; save original NMI vector
        ldx     #$01
:       lda     $0318,x
        sta     nmisave,x
        dex
        bpl     :-

; Set up the stack.

        lda     #<(__MAIN_START__ + __MAIN_SIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__)
        sta     sp
        stx     sp+1            ; Set argument stack ptr

; Switch to the second charset.

        lda     #14
        jsr     BSOUT

; set NMI vector to our exit nmi
        ldx     #<exitnmi
        lda     #>exitnmi
        jsr     setnmi

; Call the module constructors.

        jmp     initlib


; ------------------------------------------------------------------------
; Data

.segment        "INIT"

mmusave:    .res    1
spsave:     .res    1
zpsave:     .res    zpspace
stsave:     .res    1
nmisave:    .res    2
