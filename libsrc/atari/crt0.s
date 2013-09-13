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
.if .defined(__ATARIXL__)
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

.if .defined(__ATARIXL__)
.segment        "MAINHDR"
.endif

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

.if .defined(__ATARIXL__)
        jsr     sram_init
.endif

; Clear the BSS data

        jsr     zerobss

; Setup the stack

        tsx
        stx     spsave

.if .not .defined(__ATARIXL__)

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

        lda     #<(__RAM_START__ + __RAM_SIZE__ - 1)
        sta     sp
        lda     #>(__RAM_START__ + __RAM_SIZE__ - 1)
        sta     sp+1

.endif

; Call module constructors

        jsr     initlib

.if 0
.if .defined(__ATARIXL__)
        .import __heapadd
        .import pushax
        .import __RAM_BELOW_ROM_START__
        .import __RAM_BELOW_ROM_SIZE__
        .import __RAM_BELOW_ROM_LAST__

        lda     #<__RAM_BELOW_ROM_LAST__
        ldx     #>__RAM_BELOW_ROM_LAST__
        jsr     pushax
        lda     #<(__RAM_BELOW_ROM_SIZE__ - (__RAM_BELOW_ROM_LAST__ - __RAM_BELOW_ROM_START__))
        ldx     #>(__RAM_BELOW_ROM_SIZE__ - (__RAM_BELOW_ROM_LAST__ - __RAM_BELOW_ROM_START__))
        jsr     __heapadd
.endif
.endif

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

        lda     APPMHI_save
        sta     APPMHI
        lda     APPMHI_save+1
        sta     APPMHI+1

.if .defined(__ATARIXL__)

; Atari XL target stuff...

        lda     PORTB_save
        sta     PORTB
        lda     RAMTOP_save
        sta     RAMTOP
        lda     MEMTOP_save
        sta     MEMTOP
        lda     MEMTOP_save+1
        sta     MEMTOP+1


; ... issue a GRAPHICS 0 call (copied'n'pasted from TGI drivers)

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
; add error checking here...
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

spsave:         .res    1
old_shflok:     .res    1
old_lmargin:    .res    1
.if .not .defined(__ATARIXL__)
APPMHI_save:    .res    2
.endif


.segment "AUTOSTRT"
        .word   RUNAD                   ; defined in atari.inc
        .word   RUNAD+1
        .word   start
