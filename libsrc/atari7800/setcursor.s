;
; 2022-04-03, Karri Kaksonen
;
; setcursor (unsigned char on);
;
; The Atari 7800 does not have a hw cursor.
; This module emulates a cursor to be used with the conio
; implementation.
;
; The actual cursor display is included in the conio dll
; but every scanline has the element silenced by default
; at the end of every zone.
;
; The way the cursor works is to silence it before the cursor
; position changes and enable it afterwards.
;
; In order to get some performance we have a pointer to the
; cursor header structure. This structure is always at the
; end of the zone. So the pointer changes when CURS_Y changes.
;
; There is so many dependencies that it makes sense to
; deal with all CURS_X, CURS_Y stuff in this file and
; definitely not allow direct access to the variables.
;

        .export         _setcursor
        .export         gotoxy, _gotoxy, _gotox, _gotoy, wherex, wherey
	.export		CURS_X, CURS_Y
        .constructor    init_cursor

	.importzp	sp
	.import		_zones
	.import		pusha, incsp1, pusha0, pushax, popax
        .include        "atari7800.inc"
        .include        "extzp.inc"

        .macpack        generic

	.data
;-----------------------------------------------------------------------------
; The variables used by cursor functions
;

CURS_X:
	.byte	0
CURS_Y:
	.byte	0
_cursor_visible:
        .byte	1

	.code

;---------------------------------------------------------------------------
; 8x16 routine

umula0:
        ldy     #8                 ; Number of bits
	lda	#0
        lsr     ptr7800            ; Get first bit into carry
@L0:    bcc     @L1

        clc
        adc     ptrtmp
        tax
        lda     ptrtmp+1           ; hi byte of left op
        adc     ptr7800+1
        sta     ptr7800+1
        txa

@L1:    ror     ptr7800+1
        ror     a
        ror     ptr7800
        dey
        bne     @L0
        tax
        lda     ptr7800            ; Load the result
        rts

;-----------------------------------------------------------------------------
; Enable/disable cursor
;
        .proc   _setcursor

	ldx	_cursor_visible
	sta	_cursor_visible
	txa
	rts

	.endproc

;-----------------------------------------------------------------------------
; Calculate cursorzone address
; You also need to set the cursorzone to point to the correct cursor Header
; at the end of line CURS_Y.
; Offset to cursor zone 5. To next line offset 11
; cursorzone points to _zones + CURS_Y * 11 + 5
; A = CURS_Y
        .proc   calccursorzone

	sta	ptr7800
	lda	#11
	sta	ptrtmp
	lda	#0
	sta	ptr7800+1
	sta	ptrtmp+1
	jsr	umula0
	clc
	adc	#5
	bcc	@L1
	inx
@L1:	clc
	adc	#<_zones
	sta	cursorzone	; calculate new cursorzone
	txa
	adc	#>_zones
	sta	cursorzone+1
	rts

        .endproc

;-----------------------------------------------------------------------------
; Set cursor to Y position.
; You also need to set the cursorzone to point to the correct cursor Header
; at the end of line CURS_Y.
; Offset to cursor zone 5. To next line offset 11
; cursorzone points to _zones + CURS_Y * 11 + 5
;
; cursorzone[1] = 0 when not CURS_Y, 30 if CURS_Y
;
; Disable cursor
; cursorzone[1] = 0
;
; Enable cursor
; if showcursor cursorzone[1] = 30
;
        .proc   _gotoy

	pha
	lda	CURS_Y
	jsr	calccursorzone
	ldy	#1
	lda	#0
	sta	(cursorzone),y	; disable cursor
	pla
	sta	CURS_Y
	jsr	calccursorzone
	lda	_cursor_visible
	beq	@L1
	lda	#30		; enable cursor
@L1:	ldy	#1
	sta	(cursorzone),y
	rts

        .endproc

;-----------------------------------------------------------------------------
; Set cursor to X position.
; You also need to set the hpos offset to the correct value on this line
; cursorzone[3] = 8 * CURS_X
;
        .proc   _gotox

	sta	CURS_X
	ldy	#3
	clc
	rol
	rol
	rol
	sta	(cursorzone),y
	rts

        .endproc

;-----------------------------------------------------------------------------
; Set cursor to desired position (X,Y)
;
	.proc   _gotoxy

	jsr	_gotoy
	jsr	popax
	jsr	_gotox
	rts
	.endproc

	.proc   gotoxy
	jsr	popax
	jmp	_gotoxy
	.endproc
;-----------------------------------------------------------------------------
; Get cursor X position
;
	.proc   wherex

	lda	CURS_X
	jsr	pusha0
	rts
	.endproc

;-----------------------------------------------------------------------------
; Get cursor Y position
;
	.proc   wherey

	lda	CURS_Y
	jsr	pusha0
	rts
	.endproc

;-----------------------------------------------------------------------------
; Initialize cursorzone at startup
; Offset to cursor zone 5.
;
;        .segment        "ONCE"
init_cursor:
	lda	#0
	jsr	calccursorzone
	rts

;-----------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
