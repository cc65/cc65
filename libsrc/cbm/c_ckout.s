;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char __fastcall__ cbm_k_ckout (unsigned char FN);
;

       	.export	       	_cbm_k_ckout
        .import         CKOUT


_cbm_k_ckout:
	tax
       	jsr    	CKOUT
	bcs	@NotOk
	lda     #0
@NotOk:	rts
