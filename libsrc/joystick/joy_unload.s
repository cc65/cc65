;
; Ullrich von Bassewitz, 2002-11-29
;
; unsigned char __fastcall__ joy_unload (void);
; /* Unload the currently loaded driver. */


        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "modload.inc"

_joy_unload:
    	lda	_joy_drv
	ora	_joy_drv+1
       	beq    	no_driver		; No driver

	jsr	_joy_deinstall		; Deinstall the driver

        lda     _joy_drv
        ldx     _joy_drv+1
        jsr     _mod_free               ; Free the driver

        lda     #0
        sta     _joy_drv
        sta     _joy_drv+1               ; Clear the driver pointer

        tax
        rts                             ; Return zero

no_driver:
	tax	    			; X = 0
	lda	#JOY_ERR_NO_DRIVER
	rts

                    
