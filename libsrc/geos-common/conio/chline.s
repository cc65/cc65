;
; Maciej 'YTM/Elysium' Witkowiak
;
; 06.03.2002

; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);

	    .include "jumptab.inc"
	    .include "geossym.inc"

    	    .export		_chlinexy, _chline
	    .import		popa, _gotoxy, fixcursor
	    .importzp		cursor_x, cursor_y, cursor_c

_chlinexy:
       	pha	    		; Save the length
	jsr	popa		; Get y
       	jsr    	_gotoxy		; Call this one, will pop params
	pla			; Restore the length

_chline:
   	cmp	#0		; Is the length zero?
   	beq	L9  		; Jump if done
	tax
	lda	cursor_x	; left start
	sta	r3L
	lda	cursor_x+1
	sta	r3L+1
	lda	cursor_y	; level
	sec
	sbc	#4		; in the middle of a cell
	sta	r11L
	txa			; right end
	clc
	adc	cursor_c
	sta	cursor_c
	sta	r4L
	ldx	#r4
	ldy 	#3
	jsr 	DShiftLeft
	lda	#%11111111	; pattern
	jsr	HorizontalLine
	jsr	fixcursor
L9:	rts
