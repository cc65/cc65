;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char __fastcall__ cbm_k_readst (void);
;

	.include    	"cbm.inc"

       	.export	       	_cbm_k_readst

_cbm_k_readst = READST
