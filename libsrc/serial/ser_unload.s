;
; Ullrich von Bassewitz, 2003-04-15
;
; unsigned char __fastcall__ ser_unload (void);
; /* Unload the currently loaded driver. */


        .import         ser_clear_ptr

        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
        .include        "modload.inc"


_ser_unload:
    	lda	_ser_drv
	ora	_ser_drv+1
       	beq    	no_driver		; No driver

	lda	_ser_drv
	pha
	lda	_ser_drv+1
	pha				; Save pointer to driver

       	jsr    	_ser_uninstall		; Deinstall the driver
		 
	pla
	tax	
	pla				; Get pointer to driver
        jmp	_mod_free               ; Free the driver

no_driver:
       	tax				; X = 0
	lda	#<SER_ERR_NO_DRIVER
	rts

