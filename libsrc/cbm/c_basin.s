;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char __fastcall__ cbm_basin (void);
;

       	.include     	"cbm.inc"

       	.export	       	_cbm_basin

_cbm_basin:
	jsr	BASIN
	ldx	#0
	rts


