;
; Ullrich von Bassewitz, 2009-10-30
;


        .include        "tgi-kernel.inc"


;-----------------------------------------------------------------------------
; unsigned __fastcall__ tgi_textheight (const char* s);
; /* Calculate the width of the text in pixels according to the current text
;  * style.
;  */
;

.proc   _tgi_textheight

        lda     _tgi_charheight
        ldx     #0
        rts

.endproc


