;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned cbm_iobase (void);
;

       	.export	       	_cbm_iobase
        .import         IOBASE

_cbm_iobase:
	jsr	IOBASE
	txa
	pha
	tya
	tax
	pla
	rts
