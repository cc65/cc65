;
; Startup code for cc65 (PET version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         initlib, donelib
        .import         zerobss, push0
        .import         callmain
        .import         CLRCH, BSOUT
        .importzp       ST

        .include        "zeropage.inc"
        .include        "pet.inc"
        .include        "../cbm/cbm.inc"

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

; Switch to second charset. The routine that is called by BSOUT to switch the
; character set will use FNLEN as temporary storage - YUCK! Since the
; initmainargs routine, which parses the command line for arguments needs this
; information, we need to save and restore it here.
; Thanks to Stefan Haubenthal for this information!

        lda     FNLEN
        pha                     ; Save FNLEN
        lda     #14
;       sta     $E84C           ; See PET FAQ
        jsr     BSOUT
        pla
        sta     FNLEN           ; Restore FNLEN

; Clear the BSS data

        jsr     zerobss

; Save system stuff and setup the stack

        tsx
        stx     spsave          ; Save the system stack ptr

        lda     MEMSIZE
        sta     sp
        lda     MEMSIZE+1
        sta     sp+1            ; Set argument stack ptr

; Call module constructors

        jsr     initlib

; Push arguments and call main()

        jsr     callmain

; Call module destructors. This is also the _exit entry.

_exit:  pha                     ; Save the return code on stack
        jsr     donelib

; Copy back the zero page stuff

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Store the program return code into ST

        pla
        sta     ST

; Restore the stack pointer

        ldx     spsave
        txs                     ; Restore stack pointer

; Back to basic

        rts

; ------------------------------------------------------------------------

.segment        "ZPSAVE"

zpsave: .res    zpspace

; ------------------------------------------------------------------------

.bss

spsave: .res    1
mmusave:.res    1
