;
; Kevin Ruland
;
; char cgetc (void);
;
; If open_apple key is pressed then the high-bit of the key is set.
;

        .export _cgetc

        .include "apple2.inc"

_cgetc:
        lda	KBD
        bpl	_cgetc		; if < 128, no key pressed

        ; At this time, the high bit of the key pressed is set
        bit	KBDSTRB 	; clear keyboard strobe
        bit	BUTN0		; check if OpenApple is down
        bmi	:+
        and	#$7F		; If not down, then clear high bit
:       ldx	#$00
        rts
