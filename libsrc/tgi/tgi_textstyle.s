;
; Ullrich von Bassewitz, 22.06.2002
;
; void __fastcall__ tgi_textstyle (unsigned char magx, unsigned char magy,
;                                  unsigned char dir);
; /* Set the style for text output. */


        .include        "tgi-kernel.inc"

        .import         popax, incsp2
        .export         _tgi_textstyle

_tgi_textstyle:
        pha
        jsr     popax                   ; Get magx/magy in one call
        tay
        pla

; A = textdir, X = textmagx, Y = textmagy

        cmp     #TGI_TEXT_HORIZONTAL
        beq     DirOk
        cmp     #TGI_TEXT_VERTICAL
        beq     DirOk
Fail:   jmp     tgi_inv_arg             ; Invalid argument
DirOk:  cpy     #$00
        beq     Fail                    ; Cannot have magnification of zero
        cpx     #$00
        beq     Fail                    ; Cannot have magnification of zero

; Parameter check ok, store them

        stx     _tgi_textmagx
        sty     _tgi_textmagy
        sta     _tgi_textdir

; Call the driver, parameters are passed in registers

        jmp     tgi_textstyle

