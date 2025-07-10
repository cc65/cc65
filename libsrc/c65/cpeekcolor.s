;
; 2016-02-28, Groepaz
; 2017-06-22, Greg King
;
; unsigned char cpeekcolor (void);
;

        .include        "c65.inc"

        .export         _cpeekcolor
        .importzp       ptr1

_cpeekcolor:
        lda     SCREEN_PTR + 1
        clc
        adc     #>$D800
        sta     ptr1 + 1
        lda     SCREEN_PTR
        sta     ptr1

        ldy     #0
        lda     (ptr1),y

        ldx     #>$0000
        rts
