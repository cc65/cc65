;
; Ullrich von Bassewitz, 2004-03-21
;
; unsigned char mouse_unload (void);
; /* Uninstall, then unload the currently loaded driver. */


        .include        "mouse-kernel.inc"
        .include        "modload.inc"

        .import         return0



_mouse_unload:
        lda     _mouse_drv
        pha                             ; Save pointer to driver
        ora     _mouse_drv+1
        beq     no_driver               ; No driver
        lda     _mouse_drv+1
        pha

        jsr     _mouse_uninstall        ; Uninstall the driver

        pla
        tax
        pla                             ; Get pointer to driver
        jsr     _mod_free               ; Free the driver
        jmp     return0                 ; Return MOUSE_ERR_OK

no_driver:
        tax                             ; X = 0
        pla                             ; Remove pushed junk
        lda     #<MOUSE_ERR_NO_DRIVER
        rts
