;
; Ullrich von Bassewitz, 2004-03-21
;
; unsigned char __fastcall__ mouse_unload (void);
; /* Uninstall, then unload the currently loaded driver. */


        .include        "mouse-kernel.inc"
        .include        "modload.inc"


_mouse_unload:
      	lda	_mouse_drv
	ora	_mouse_drv+1
       	beq    	no_driver		; No driver

	lda	_mouse_drv
	pha
	lda	_mouse_drv+1
	pha	      			; Save pointer to driver

       	jsr    	_mouse_uninstall        ; Uninstall the driver

	pla
	tax
	pla				; Get pointer to driver
        jmp	_mod_free               ; Free the driver

no_driver:
       	tax				; X = 0
	lda	#<MOUSE_ERR_NO_DRIVER
	rts

