;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_unload (void);
; /* Unload the currently loaded driver. */


        .include        "tgi-kernel.inc"
        .include        "modload.inc"

        .import         _tgi_done
        .export         _tgi_unload


_tgi_unload:
        jsr     _tgi_done               ; Switch off graphics
        jsr     tgi_deinstall           ; Allow the driver to clean up

        lda     _tgi_drv
        ldx     _tgi_drv+1
        jsr     _mod_free               ; Free the driver

; Clear variables

        lda     #$00
        sta     _tgi_drv
        sta     _tgi_drv+1
        sta     _tgi_error
        
        rts


