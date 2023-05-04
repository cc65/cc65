;
; Ullrich von Bassewitz, 2002-11-29
;
; unsigned char joy_unload (void);
; /* Unload the currently loaded driver. */


        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "modload.inc"

        .import         joy_clear_ptr
        .import         return0, return1



_joy_unload:
        lda     _joy_drv
        pha                             ; Save pointer to driver
        ora     _joy_drv+1
        beq     no_driver               ; No driver
        lda     _joy_drv+1
        pha

        jsr     _joy_uninstall          ; Uninstall the driver

        pla
        tax
        pla                             ; Get pointer to driver
        jsr     _mod_free               ; Free the driver
        jmp     return0                 ; Return JOY_ERR_OK

no_driver:
        pla                             ; Remove pushed junk
        .assert JOY_ERR_NO_DRIVER = 1, error
        jmp     return1                 ; Return JOY_ERR_NO_DRIVER
