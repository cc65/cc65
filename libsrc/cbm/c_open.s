;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char __fastcall__ cbm_k_open (void);
;

	.include    	"cbm.inc"

       	.export	       	_cbm_k_open

_cbm_k_open:
	jsr	OPEN
	bcs	@NotOk
        lda     #0
@NotOk:	ldx     #0              ; Clear high byte
        rts
