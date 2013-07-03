;
; Startup code for cc65 (ATARI version)
;
; Contributing authors:
;       Mark Keates
;       Freddy Offenga
;       Christian Groessler
;       Stefan Haubenthal
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         callmain, zerobss
        .import         __STARTUP_LOAD__, __BSS_LOAD__
        .import         __RESERVED_MEMORY__

        .include        "zeropage.inc"
        .include        "atari.inc"

; ------------------------------------------------------------------------
; EXE header

.segment        "EXEHDR"

        .word   $FFFF
        .word   __STARTUP_LOAD__
        .word   __BSS_LOAD__ - 1

; ------------------------------------------------------------------------
; Actual code

.segment        "STARTUP"

        rts     ; fix for SpartaDOS / OS/A+
                ; they first call the entry point from AUTOSTRT and
                ; then the load addess (this rts here).
                ; We point AUTOSTRT directly after the rts.

; Real entry point:

; Clear the BSS data

        jsr     zerobss

; Setup the stack

        tsx
        stx     spsave

; Report memory usage

        lda     APPMHI
        sta     appmsav                 ; remember old APPMHI value
        lda     APPMHI+1
        sta     appmsav+1

        sec
        lda     MEMTOP
        sbc     #<__RESERVED_MEMORY__
        sta     APPMHI                  ; initialize our APPMHI value
        sta     sp                      ; setup runtime stack part 1
        lda     MEMTOP+1
        sbc     #>__RESERVED_MEMORY__
        sta     APPMHI+1
        sta     sp+1                    ; setup runtime stack part 2

; Call module constructors

        jsr     initlib

; Set left margin to 0

        lda     LMARGN
        sta     old_lmargin
        ldy     #0
        sty     LMARGN

; Set keyb to upper/lowercase mode

        ldx     SHFLOK
        stx     old_shflok
        sty     SHFLOK

; Initialize conio stuff

        dey                             ; Set X to $FF
        sty     CH

; Push arguments and call main

        jsr     callmain

; Call module destructors. This is also the _exit entry.

_exit:  jsr     donelib         ; Run module destructors

; Restore system stuff

        ldx     spsave
        txs                     ; Restore stack pointer

; Restore left margin

        lda     old_lmargin
        sta     LMARGN

; Restore kb mode

        lda     old_shflok
        sta     SHFLOK

; Restore APPMHI

        lda     appmsav
        sta     APPMHI
        lda     appmsav+1
        sta     APPMHI+1

; Turn on cursor

        ldx     #0
        stx     CRSINH

; Back to DOS

        rts

; *** end of main startup code

; ------------------------------------------------------------------------

.bss

spsave:         .res    1
appmsav:        .res    1
old_shflok:     .res    1
old_lmargin:    .res    1


.segment "AUTOSTRT"
        .word   RUNAD                   ; defined in atari.inc
        .word   RUNAD+1
        .word   __STARTUP_LOAD__ + 1
