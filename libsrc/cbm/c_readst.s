;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned __fastcall__ cbm_readst (void);
;

	.include    	"cbm.inc"

       	.export	       	_cbm_readst

_cbm_readst:
	jsr	READST
	ldx	#0
	rts

