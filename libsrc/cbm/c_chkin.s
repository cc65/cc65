;
; Marc 'BlackJack' Rintsch, 20.03.2001
;
; unsigned char __fastcall__ cbm_k_chkin (unsigned char FN);
;

       	.include     	"cbm.inc"

       	.export	       	_cbm_k_chkin

_cbm_k_chkin:
	tax
       	jsr    	CHKIN
	bcs	@NotOk
        lda     #0
@NotOk:	rts

