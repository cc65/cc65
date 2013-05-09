;
; Ullrich von Bassewitz, 2002-11-29
;
; unsigned char em_unload (void);
; /* Unload the currently loaded driver. */


        .import         em_clear_ptr

        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .include        "modload.inc"

_em_unload:
        lda     _em_drv
        ora     _em_drv+1
        beq     no_driver               ; No driver

        jsr     emd_uninstall           ; Deinstall the driver

        lda     _em_drv
        ldx     _em_drv+1
        jsr     _mod_free               ; Free the driver

        jmp     em_clear_ptr            ; Clear the driver pointer, return zero

no_driver:
        tax                             ; X = 0
        lda     #EM_ERR_NO_DRIVER
        rts
