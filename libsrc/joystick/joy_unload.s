;
; Ullrich von Bassewitz, 2002-11-29
;
; unsigned char __fastcall__ joy_unload (void);
; /* Unload the currently loaded driver. */


        .import         joy_clear_ptr

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "modload.inc"

_joy_unload:
    	lda	_joy_drv
	ora	_joy_drv+1
       	beq    	no_driver		; No driver

       	jsr    	joy_uninstall		; Uninstall the driver

        lda     _joy_drv
        ldx     _joy_drv+1
        jsr     _mod_free               ; Free the driver

        jmp     joy_clear_ptr           ; Clear driver pointer, return zero

no_driver:
	tax	    			; X = 0
	lda	#JOY_ERR_NO_DRIVER
	rts


