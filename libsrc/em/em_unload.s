;
; Ullrich von Bassewitz, 2002-11-29
;
; unsigned char __fastcall__ em_unload (void);
; /* Unload the currently loaded driver. */


        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .include        "modload.inc"

	.import		return0


_em_unload:
	lda	_em_drv
	ora	_em_drv+1
       	beq    	no_driver		; No driver

	jsr	_em_deinstall		; Deinstall the driver

        lda     _em_drv
        ldx     _em_drv+1
        jsr     _mod_free               ; Free the driver

	jmp	return0

no_driver:
	tax				; X = 0
	lda	#EM_ERR_NO_DRIVER
	rts

