;
; Karri Kaksonen, 2004-11-01
;
; char cgetc (void);
;

       	.export		_cgetc
       	.import		_kbhit
       	.import		KBBUF

; --------------------------------------------------------------------------
; The Atari Lynx has a very small keyboard - only 3 keys
; Opt1, Opt2 and Pause.
; But the designers have decided that pressing Pause and Opt1 at the
; same time means Restart and pressing Pause and Opt2 means Flip screen.

; For "easter egg" use I have also included all three keys pressed '?'
; and Opt1 + Opt2 pressed '3'.
; So the keyboard returns '1', '2', '3', 'P', 'R', 'F' or '?'.

_cgetc:
    	jsr    	_kbhit	       	; Check for char available
        tax                     ; Test result
	beq	_cgetc

	ldx	#5		; Wait for some time... 0.2 seconds or so.
@L1:	ldy	#255
@L2:	lda	#255
@L3:	dec
	bne	@L3
	dey
	bne	@L2
	dex
	bne	@L1

    	jsr    	_kbhit	       	; Check for double pressed buttons
	lda	KBBUF
	stz	KBBUF
       	ldx	#0
       	rts

