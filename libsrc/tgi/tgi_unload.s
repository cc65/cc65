;
; Ullrich von Bassewitz, 21.06.2002
;
; void tgi_unload (void);
; /* Unload the currently loaded driver. */


        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"
        .include        "modload.inc"


_tgi_unload:

        lda     _tgi_drv
        ora     _tgi_drv
        beq     no_driver               ; No driver

        lda     _tgi_drv
        pha
        lda     _tgi_drv+1
        pha                             ; Save pointer to driver

        jsr     _tgi_uninstall          ; Uninstall the driver

        pla
        tax
        pla
        jmp     _mod_free               ; Free the driver

no_driver:
        lda     #<TGI_ERR_NO_DRIVER
        sta     _tgi_error
        rts
