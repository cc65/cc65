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
       	beq    	no_driver	   	; No driver

	lda	_joy_drv
	pha
	lda	_joy_drv+1
	pha	      		   	; Save pointer to driver

       	jsr    	_joy_uninstall          ; Uninstall the driver

	pla
	tax
	pla			   	; Get pointer to driver
        jmp	_mod_free               ; Free the driver

no_driver:
	tax	    		   	; X = 0
	lda	#JOY_ERR_NO_DRIVER
	rts


