;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned __fastcall__ cbm_iobase (void);
;

       	.include     	"cbm.inc"

       	.export	       	_cbm_iobase

_cbm_iobase:
	jsr	IOBASE
	txa
	pha
	tya
	tax
	pla
	rts


