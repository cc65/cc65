;
; Christian Groessler, 19-Feb-2000
;
; void gotoy (unsigned char y);
;

	.include	"atari.inc"
	.export		_gotoy

_gotoy:
	sta	ROWCRS		; Set Y
	rts
