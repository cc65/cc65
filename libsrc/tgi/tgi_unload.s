;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_unload (void);
; /* Unload the currently loaded driver. */


        .include        "tgi-kernel.inc"
        .include        "modload.inc"

        .import         tgi_clear_ptr

.proc   _tgi_unload

        jsr     _tgi_done               ; Switch off graphics
        jsr     tgi_uninstall           ; Allow the driver to clean up

        lda     _tgi_drv
        ldx     _tgi_drv+1
        jsr     _mod_free               ; Free the driver

        jmp     tgi_clear_ptr           ; Clear the driver pointer and exit

.endproc

