;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned __fastcall__ cbm_ckout (unsigned char FN);
;

       	.include     	"cbm.inc"

       	.export	       	_cbm_ckout

_cbm_ckout:
	tax
       	jsr    	CKOUT
	ldx	#0
	bcc	@Ok
	inx
	rts
@Ok:	txa
	rts





