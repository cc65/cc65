;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char wherey (void);
;

 	.export		_wherey
        .import         PLOT


.proc   _wherey
	sec
	jsr	PLOT		; Get cursor position
	txa
        ldx     #$00
	rts
.endproc

