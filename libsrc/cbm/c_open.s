;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned __fastcall__ cbm_open (void);
;

	.include    	"cbm.inc"

       	.export	       	_cbm_open

_cbm_open:
	jsr	OPEN
	ldx	#0
	bcc	@Ok
	inx
	rts
@Ok:	txa
	rts

