;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char __fastcall__ cbm_acptr (void);
;

	.include    	"cbm.inc"

       	.export	       	_cbm_acptr

_cbm_acptr:
	jsr	ACPTR
	ldx	#0
	rts

		     
