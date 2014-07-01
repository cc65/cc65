;
; Ullrich von Bassewitz, 2009-11-06
;

        .include        "tgi-kernel.inc"


;-----------------------------------------------------------------------------
; void __fastcall__ tgi_install_vectorfont (const tgi_vectorfont* font);
; /* Install a vector font for use. More than one vector font can be loaded,
; ** but only one can be active. This function is used to tell which one. Call
; ** with a NULL pointer to uninstall the currently installed font.
; */
;

.code
.proc   _tgi_install_vectorfont

        sta     _tgi_vectorfont
        stx     _tgi_vectorfont+1
        rts

.endproc

