;
; Karri Kaksonen, 2004-11-01
;
; unsigned char kbhit (void);
;

	.export		_kbhit
	.export		KBBUF
	.import		return0, return1

; --------------------------------------------------------------------------
; The Atari Lynx has a very small keyboard - only 3 keys
; Opt1, Opt2 and Pause.
; But the designers have decided that pressing Pause and Opt1 at the
; same time means Restart and pressing Pause and Opt2 means Flip screen.

; For "easter egg" use I have also included all three keys pressed '?'
; and Opt1 + Opt2 pressed '3'.
; So the keyboard returns '1', '2', '3', 'P', 'R', 'F' or '?'.

	.data
KBBUF:		.byte	0
DEBOUNCE:	.byte	0	; Contains char until key is freed

	.code
_kbhit:
	lda	$FCB1		; Read the Pause key
	and	#1
	bne	@L4
	lda	$FCB0		; No Pause pressed
	and	#$0c
	bne	@L1
	stz	DEBOUNCE	; No keys pressed at all
	lda	KBBUF		; But we may have some old key in the buffer
	bne	@L9
	jmp	return0		; No key has been pressed
@L1:
	cmp	#$0c
	bne	@L2
	ldx	#'3'		; Opt 1 + Opt 2 pressed
	bra	@L8
@L2:
	cmp	#$08
	bne	@L3
	ldx	#'1'		; Opt 1 pressed
	bra	@L8
@L3:
	ldx	#'2'		; Opt 2 pressed
	bra	@L8
@L4:
	lda	$FCB0
	and	#$0c
	bne	@L5
	ldx	#'P'		; Pause pressed
	bra	@L8
@L5:
	cmp	#$0c
	bne	@L6
	ldx	#'?'	; Opt 1 + Opt 2 + Pause pressed
	bra	@L8
@L6:
	cmp	#$08
	bne	@L7
	ldx	#'R'	; Restart pressed
	bra	@L8
@L7:
	ldx	#'F'	; Flip pressed
@L8:
	lda	KBBUF
	bne	@L10
	lda	DEBOUNCE
	beq	@L10
	jmp	return0	; Return no key pressed until keys are released
@L10:
	stx	KBBUF
	sta	DEBOUNCE
@L9:	jmp	return1

                
