;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char wherex (void);
; unsigned char wherey (void);

	.export		_wherex, _wherey
	.import		plot
	
	.include	"atari.inc"

_wherex:
	sec
	jsr	plot		; Get cursor position
	tya
	rts

_wherey:
	sec
	jsr	plot		; Get cursor position
	txa
	rts





