;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char wherex (void);
; unsigned char wherey (void);


	.export		_wherex, _wherey
        .import         PLOT


_wherex:
	sec
	jsr	PLOT		; Get cursor position
	tya
	rts

_wherey:
	sec
	jsr	PLOT		; Get cursor position
	txa
	rts





