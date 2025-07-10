;
; 2016-02-28, Groepaz
; 2017-06-15, Greg King
;
; unsigned char cpeekrevers (void);
;

        .include        "mega65.inc"

        .export         _cpeekrevers
        .importzp       ptr1

_cpeekrevers:
        lda     SCREEN_PTR + 1
        clc
        adc     #>$0800
        sta     ptr1 + 1
        lda     SCREEN_PTR
        sta     ptr1

        ldy     CURS_X
        lda     (ptr1),y  ; get screen code
        and     #$80            ; get reverse bit
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value

        rts
