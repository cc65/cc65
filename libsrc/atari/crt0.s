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
        .import         __STARTUP_LOAD__, __ZPSAVE_LOAD__, __BSS_LOAD__
        .import         __RESERVED_MEMORY__
	.import		__RAM_START__, __RAM_SIZE__
	.import		zpsave
	.import		sram_init
.if .defined(__ATARIXL__)
	.import		scrdev
.endif

        .include        "zeropage.inc"
        .include        "atari.inc"
	.include	"save_area.inc"

; ------------------------------------------------------------------------
; EXE header

.segment        "EXEHDR"

        .word   $FFFF

.if .defined(__ATARIXL__)
.segment	"MAINHDR"
.endif

        .word   __STARTUP_LOAD__
.if .defined(__ATARIXL__)
        .word   __BSS_LOAD__ - 1
.else
        .word   __ZPSAVE_LOAD__ - 1
.endif

; ------------------------------------------------------------------------
; Actual code

.segment        "STARTUP"

        rts     ; fix for SpartaDOS / OS/A+
                ; they first call the entry point from AUTOSTRT and
                ; then the load addess (this rts here).
                ; We point AUTOSTRT directly after the rts.

; Real entry point:

.if .not .defined(__ATARIXL__)		; already done in previous load chunk

; Save the zero page locations we need

        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1

.else

	jsr	sram_init

.endif

; Clear the BSS data

        jsr     zerobss

; Setup the stack

        tsx
        stx     spsave

.if .not .defined(__ATARIXL__)

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

.else

	lda	#<(__RAM_START__ + __RAM_SIZE__ - 1)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__ - 1)
	sta	sp+1

.endif

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

.if .not .defined(__ATARIXL__)

; Restore APPMHI

        lda     appmsav
        sta     APPMHI
        lda     appmsav+1
        sta     APPMHI+1

.else

; Atari XL target stuff...

	lda	PORTB_save
	sta	PORTB
	lda	RAMTOP_save
	sta	RAMTOP
	lda	MEMTOP_save
	sta	MEMTOP
	lda	MEMTOP_save+1
	sta	MEMTOP+1
	lda	APPMHI_save
	sta	APPMHI
	lda	APPMHI_save+1
	sta	APPMHI+1



; ... issue a GRAPHICS 0 call (copied'n'pasted from TGI drivers)


	ldx	#$50		; take any IOCB, hopefully free (@@@ fixme)

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



.endif


; Copy back the zero page stuff

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Turn on cursor

        inx
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
appmsav:        .res    1
.endif

        .segment "AUTOSTRT"
        .word   RUNAD                   ; defined in atari.h
        .word   RUNAD+1
        .word   __STARTUP_LOAD__ + 1
