;
; Ullrich von Bassewitz, 03.06.1999
;
; void __fastcall__ cbm_setlfs (unsigned char LFN,
;				unsigned char DEV,
;				unsigned char SA);
;

       	.include     	"cbm.inc"

       	.export	       	_cbm_setlfs
	.import		popa
	.importzp	tmp1

_cbm_setlfs:
  	sta	tmp1		; Save SA
	jsr	popa		; Get DEV
	tax
	jsr	popa		; Get LFN
	ldy	tmp1		; Get SA
	jmp	SETLFS


