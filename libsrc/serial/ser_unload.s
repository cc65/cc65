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

	jsr	ser_uninstall		; Deinstall the driver

        lda     _ser_drv
        ldx     _ser_drv+1
        jsr     _mod_free               ; Free the driver

        jmp     ser_clear_ptr           ; Clear the driver pointer, return zero

no_driver:
	tax				; X = 0
	lda	#SER_ERR_NO_DRIVER
	rts

