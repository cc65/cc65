
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 10.09.2001

; unsigned char get_ostype (void);
; unsigned char get_tv (void);

	    .export _get_ostype
	    .export _get_tv
	    .importzp tmp1

	    .include "../inc/const.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/geossym2.inc"

_get_ostype:
	    ldx #0
	    lda version
	    and #%11110000
	    cmp #$10
	    beq geos10
	    lda c128Flag		; we're on at least 2.0
	    ora version
	    rts
geos10:
	    lda version
	    rts

_get_tv:
	    jsr _get_ostype
	    bpl only40			; C64 with 40 columns only
	    lda graphMode
	    bpl only40			; C128 but currently on 40 columns
	    ldx #1			; COLUMNS80
	    bne tvmode
only40:	    ldx #0			; COLUMNS40
tvmode:					; PAL/NTSC check here, result in A
            php
	    sei                         ; disable interrupts
	    lda CPU_DATA		; this is for C64
	    pha
	    lda #IO_IN			; enable access to I/O
	    sta CPU_DATA
            bit rasreg
	    bpl tvmode			; wait for rasterline  127<x<256
	    lda #24			; (rasterline now >=256!)
modelp:	    cmp rasreg			; wait for rasterline = 24 (or 280 on PAL)
	    bne modelp
	    lda grcntrl1		; 24 or 280 ?
	    bpl ntsc
	    lda #0			; PAL
	    beq modeend
ntsc:	    lda #$80			; NTSC

modeend:    stx tmp1
	    ora tmp1
	    sta tmp1
	    ldx #0
	    pla
	    sta CPU_DATA		; restore memory config
	    plp                         ; restore interrupt state
	    lda tmp1
	    rts
