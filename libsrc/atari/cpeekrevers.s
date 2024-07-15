;
; 2017-06-21, Greg King
;
; unsigned char cpeekrevers (void);
;

        .export         _cpeekrevers

        .include        "atari.inc"


_cpeekrevers:
        lda     OLDCHR          ; get char under cursor
        and     #$80            ; get reverse bit
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value
        rts
