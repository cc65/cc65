;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char wherex (void);
;

  	.export		_wherex
        .import         PLOT


.proc   _wherex
  	sec
  	jsr	PLOT		; Get cursor position
  	tya
        ldx     #$00
  	rts
.endproc
        
