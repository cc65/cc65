;
; Ullrich von Bassewitz, 2011-07-17
;

        .include        "tgi-kernel.inc"


;-----------------------------------------------------------------------------
; void __fastcall__ tgi_settextdir (unsigned char dir);
; /* Set the direction for text output. dir is one of the TGI_TEXT_XXX
; ** constants.
; */
;

.proc   _tgi_settextdir

        sta     _tgi_textdir            ; Remember the direction
        rts

.endproc

