	;;
	;; Kevin Ruland
	;;
	;; char cgetc (void);
	;;
	;; If open_apple key is pressed then the high-bit of the
	;; key is set.
	
	.export _cgetc

	.include "apple2.inc"

_cgetc:
	lda	KEY_STROBE
	bpl	_cgetc		; if < 128, no key pressed
	;; At this time, the high bit of the key pressed
	;; is set
	sta	CLEAR_KEY_STROBE; clear keyboard strobe
	bit	OPEN_APPLE	; check if OpenApple is down
	bmi	pressed	
	and	#$7F		; If not down, then clear high bit
pressed:	
	ldx	#0
	rts
	