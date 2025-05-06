;
; Ullrich von Bassewitz, 2003-04-15
;
; unsigned char ser_unload (void);
; /* Unload the currently loaded driver. */


        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
        .include        "modload.inc"

        .import         ser_clear_ptr
        .import         return0, return1



_ser_unload:
        lda     _ser_drv
        pha                             ; Save pointer to driver
        ora     _ser_drv+1
        beq     no_driver               ; No driver
        lda     _ser_drv+1
        pha

        jsr     _ser_uninstall          ; Uninstall the driver

        pla
        tax
        pla                             ; Get pointer to driver
        jsr     _mod_free               ; Free the driver
        .assert SER_ERR_OK = 0, error
        jmp     return0

no_driver:
        pla                             ; Remove pushed junk
        .assert SER_ERR_NO_DRIVER = 1, error
        jmp     return1
