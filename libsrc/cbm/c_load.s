;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned __fastcall__ cbm_load (unsigned char flag, unsigned addr);
;

	.include    	"cbm.inc"

       	.export	       	_cbm_load
	.import		popa
	.importzp	ptr1

_cbm_load:
	sta	ptr1
	stx	ptr1+1	    
	jsr	popa		; get flag
	ldx	ptr1
	ldy	ptr1+1
	jsr	LOAD
	ldx	#0
	bcc	@Ok
	inx
	rts
@Ok:	txa
	rts

