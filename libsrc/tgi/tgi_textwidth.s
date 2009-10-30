;
; Ullrich von Bassewitz, 2009-10-30
;


        .include        "tgi-kernel.inc"

        .import         _strlen, pushax, tosumula0

;-----------------------------------------------------------------------------
; unsigned __fastcall__ tgi_textwidth (const char* s);
; /* Calculate the width of the text in pixels according to the current text
;  * style.
;  */
;
; Result is  strlen (s) * tgi_textmagw * tgi_fontsizex
;

.proc   _tgi_textwidth

        jsr     _strlen
        jsr     pushax
        lda     _tgi_charwidth
        jmp     tosumula0

.endproc

