
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; unsigned char revers (unsigned char onoff);

	    .export _revers
	    .import tmp1

	    .include "../inc/geossym.inc"
	    .include "../inc/const.inc"

_revers:
	    tax
	    bne L0			; turn on
	    lda #0
	    .byte $2c
L0:	    lda #SET_REVERSE
	    sta tmp1

	    lda currentMode
	    tax
	    and #SET_REVERSE
	    tay				; store old value
	    txa
	    and #%11011111		; mask out
	    ora tmp1			; set new value
	    sta currentMode

	    ldx #0
	    tya
	    beq L1
	    lda #1
L1:	    rts
