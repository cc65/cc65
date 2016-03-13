;
; Startup code for cc65 (ATARI version)
;
; Contributing authors:
;       Mark Keates
;       Freddy Offenga
;       Christian Groessler
;       Stefan Haubenthal
;

        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .export         _exit, start

        .import         initlib, donelib
        .import         callmain, zerobss
        .import         __RESERVED_MEMORY__
        .import         __MAIN_START__, __MAIN_SIZE__
.ifdef __ATARIXL__
        .import         __STACKSIZE__
        .import         sram_init
        .import         scrdev
        .import         findfreeiocb
        .forceimport    sramprep                        ; force inclusion of the "shadow RAM preparation" load chunk
        .include        "save_area.inc"
.endif

        .include        "zeropage.inc"
        .include        "atari.inc"

; ------------------------------------------------------------------------

.segment        "STARTUP"

        rts     ; fix for SpartaDOS / OS/A+
                ; They first call the entry point from AUTOSTRT; and
                ; then, the load address (this rts here).
                ; We point AUTOSTRT directly after the rts.

; Real entry point:

start:

.ifdef __ATARIXL__
        jsr     sram_init
.endif

; Clear the BSS data.

        jsr     zerobss

; Set up the stack.

        tsx
        stx     SP_save

.ifdef __ATARIXL__

        lda     #<(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        sta     sp
        stx     sp+1

.else

; Report the memory usage.

        lda     APPMHI
        ldx     APPMHI+1
        sta     APPMHI_save             ; remember old APPMHI value
        stx     APPMHI_save+1

        sec
        lda     MEMTOP
        sbc     #<__RESERVED_MEMORY__
        sta     APPMHI                  ; initialize our APPMHI value
        sta     sp                      ; set up runtime stack part 1
        lda     MEMTOP+1
        sbc     #>__RESERVED_MEMORY__
        sta     APPMHI+1
        sta     sp+1                    ; set up runtime stack part 2

.endif

; Call the module constructors.

        jsr     initlib

; Set the left margin to 0.

        lda     LMARGN
        sta     LMARGN_save
        ldy     #0
        sty     LMARGN

; Set the keyboard to upper-/lower-case mode.

        ldx     SHFLOK
        stx     SHFLOK_save
        sty     SHFLOK

; Initialize the conio stuff.

        dey                     ; Set Y to $FF
        sty     CH              ; remove keypress which might be in the input buffer

; Push the command-line arguments; and, call main().

        jsr     callmain

; Call the module destructors. This is also the exit() entry.

_exit:  jsr     donelib         ; Run module destructors

; Restore the system stuff.

        ldx     SP_save
        txs                     ; Restore stack pointer

; Restore the left margin.

        lda     LMARGN_save
        sta     LMARGN

; Restore the kb mode.

        lda     SHFLOK_save
        sta     SHFLOK

; Restore APPMHI.

        lda     APPMHI_save
        ldx     APPMHI_save+1
        sta     APPMHI
        stx     APPMHI+1

.ifdef __ATARIXL__

; Atari XL target stuff...

        lda     PORTB_save
        sta     PORTB
        lda     RAMTOP_save
        sta     RAMTOP
        lda     MEMTOP_save
        ldx     MEMTOP_save+1
        sta     MEMTOP
        stx     MEMTOP+1


; Issue a GRAPHICS 0 call (copied'n'pasted from the TGI drivers), in
; order to restore screen memory to its default location just
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
; No error checking here, shouldn't happen(TM); and, no way to
; recover anyway.

        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV_org

.endif

; Turn on the cursor.

        ldx     #0
        stx     CRSINH

; Back to DOS.

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
