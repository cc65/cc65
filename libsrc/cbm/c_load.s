;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char __fastcall__ cbm_k_load (unsigned char flag, unsigned addr);
;

	.include    	"cbm.inc"

       	.export	       	_cbm_k_load
	.import		popa
	.importzp	ptr1

_cbm_k_load:
	sta	ptr1
	stx	ptr1+1
	jsr	popa		; get flag
	ldx	ptr1
	ldy	ptr1+1
	jsr	LOAD
	bcs	@NotOk
	lda     #0
@NotOk:	rts
	
