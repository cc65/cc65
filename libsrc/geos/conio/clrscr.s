
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; void clrscr (void);

	    .export _clrscr

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	    .include "../inc/const.inc"

_clrscr:
	    lda #ST_WR_FORE | ST_WR_BACK
	    sta dispBufferOn
	    lda #0
	    jsr SetPattern
	    lda #0
	    sta r3L
	    sta r3H
	    sta r2L
	    lda #199
	    sta r2H
	    lda graphMode
	    bpl L40
	    lda #>639			; 80 columns
	    ldx #<639
	    bne L99
L40:	    lda #>319			; 40 columns
	    ldx #<319
L99:	    sta r4H
	    stx r4L
	    jmp Rectangle
