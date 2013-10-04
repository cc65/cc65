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
        .import         __RAM_START__, __RAM_SIZE__
.ifdef __ATARIXL__
        .import         __STACKSIZE__
        .import         sram_init
        .import         scrdev
        .import         findfreeiocb
        .include        "save_area.inc"
.endif

        .include        "zeropage.inc"
        .include        "atari.inc"

; ------------------------------------------------------------------------
; EXE header

.segment        "EXEHDR"

        .word   $FFFF

.segment        "MAINHDR"

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

start:

.ifdef __ATARIXL__
        jsr     sram_init
.endif

; Clear the BSS data

        jsr     zerobss

; Setup the stack

        tsx
        stx     SP_save

.ifndef __ATARIXL__

; Report memory usage

        lda     APPMHI
        sta     APPMHI_save             ; remember old APPMHI value
        lda     APPMHI+1
        sta     APPMHI_save+1

        sec
        lda     MEMTOP
        sbc     #<__RESERVED_MEMORY__
        sta     APPMHI                  ; initialize our APPMHI value
        sta     sp                      ; setup runtime stack part 1
        lda     MEMTOP+1
        sbc     #>__RESERVED_MEMORY__
        sta     APPMHI+1
        sta     sp+1                    ; setup runtime stack part 2

.else

        lda     #<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp
        lda     #>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp+1

.endif

; Call module constructors

        jsr     initlib

; Set left margin to 0

        lda     LMARGN
        sta     LMARGN_save
        ldy     #0
        sty     LMARGN

; Set keyb to upper/lowercase mode

        ldx     SHFLOK
        stx     SHFLOK_save
        sty     SHFLOK

; Initialize conio stuff

        dey                             ; Set X to $FF
        sty     CH

; Push arguments and call main

        jsr     callmain

; Call module destructors. This is also the _exit entry.

_exit:  jsr     donelib         ; Run module destructors

; Restore system stuff

        ldx     SP_save
        txs                     ; Restore stack pointer

; Restore left margin

        lda     LMARGN_save
        sta     LMARGN

; Restore kb mode

        lda     SHFLOK_save
        sta     SHFLOK

; Restore APPMHI

        lda     APPMHI_save
        sta     APPMHI
        lda     APPMHI_save+1
        sta     APPMHI+1

.ifdef __ATARIXL__

; Atari XL target stuff...

        lda     PORTB_save
        sta     PORTB
        lda     RAMTOP_save
        sta     RAMTOP
        lda     MEMTOP_save
        sta     MEMTOP
        lda     MEMTOP_save+1
        sta     MEMTOP+1


; Issue a GRAPHICS 0 call (copied'n'pasted from TGI drivers) in
; order to restore screen memory to its defailt location just
; before the ROM.

        jsr     findfreeiocb

        ; Reopen it in Graphics 0
        lda     #OPEN
        sta     ICCOM,x
        lda     #OPNIN | OPNOT
        sta     ICAX1,x
        lda     #0
        sta     ICAX2,x
        lda     #<scrdev
        sta     ICBAL,x
        lda     #>scrdev
        sta     ICBAH,x
        lda     #3
        sta     ICBLL,x
        lda     #0
        sta     ICBLH,x
        jsr     CIOV_org
; No error checking here, shouldn't happen(tm), and no way to
; recover anyway.

        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV_org

.endif

; Turn on cursor

        ldx     #0
        stx     CRSINH

; Back to DOS

        rts

; *** end of main startup code

; ------------------------------------------------------------------------

.bss

SP_save:        .res    1
SHFLOK_save:    .res    1
LMARGN_save:    .res    1
.ifndef __ATARIXL__
APPMHI_save:    .res    2
.endif


.segment "AUTOSTRT"
        .word   RUNAD                   ; defined in atari.inc
        .word   RUNAD+1
        .word   start
