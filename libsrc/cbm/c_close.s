;
; Ullrich von Bassewitz, 03.06.1999
;
; void __fastcall__ cbm_close (unsigned char FN);
;

       	.include     	"cbm.inc"

       	.export	       	_cbm_close

_cbm_close:
       	clc
       	jmp    	CLOSE


