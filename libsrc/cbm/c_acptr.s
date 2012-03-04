;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned char cbm_acptr (void);
;

       	.export	       	_cbm_acptr
        .import         ACPTR


_cbm_acptr:
	jsr	ACPTR
	ldx	#0
	rts
