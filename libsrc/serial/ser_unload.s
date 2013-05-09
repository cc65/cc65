;
; Ullrich von Bassewitz, 2003-04-15
;
; unsigned char ser_unload (void);
; /* Unload the currently loaded driver. */


        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
        .include        "modload.inc"

        .import         ser_clear_ptr
        .import         return0



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
        jmp     return0                 ; Return SER_ERR_OK

no_driver:
        tax                             ; X = 0
        pla                             ; Remove pushed junk
        lda     #<SER_ERR_NO_DRIVER
        rts
