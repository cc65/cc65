;
; Ullrich von Bassewitz, 22.06.2002
;


        .include        "tgi-kernel.inc"

        .import         _strlen, pushax, tosumulax

;-----------------------------------------------------------------------------
; unsigned __fastcall__ tgi_textwidth (const char* s);
; /* Calculate the width of the text in pixels according to the current text
;  * style.
;  */


_tgi_textwidth:

        ldy     _tgi_textdir            ; Get text direction
        bne     height

; Result is
;
;       strlen (s) * tgi_textmagx * tgi_fontsizex
;
; Since we don't expect textmagx to have large values, we do the multiplication
; by looping.

width:  jsr     _strlen
        jsr     pushax

        lda     #0
        tax
        ldy     _tgi_textmagx
@L1:    clc
        adc     _tgi_fontsizex
        bcc     @L2
        inx
@L2:    dey
        bne     @L1

        jmp     tosumulax               ; Result * strlen (s)

;-----------------------------------------------------------------------------
; unsigned __fastcall__ tgi_textheight (const char* s);
; /* Calculate the height of the text in pixels according to the current text
;  * style.
;  */

_tgi_textheight:
        ldy     _tgi_textdir            ; Get text direction
        bne     width                   ; Jump if vertical

; Result is
;
;       tgi_textmagy * tgi_fontsizey
;
; Since we don't expect textmagx to have large values, we do the multiplication
; by looping.

height: lda     #0
        tax
        ldy     _tgi_textmagy
@L1:    clc
        adc     _tgi_fontsizey
        bcc     @L2
        inx
@L2:    dey
        bne     @L1
        rts


