;
; 2019-11-06, Greg King
;
; unsigned char wherey (void);
;

        .export         _wherey

        .include        "cx16.inc"

.proc   _wherey
        lda     CURS_Y
        ldx     #>$0000
        rts
.endproc
