;
; 2019-11-06, Greg King
;
; unsigned char wherex (void);
;

        .export         _wherex

        .include        "cx16.inc"

.proc   _wherex
        lda     CURS_X
        ldx     #>$0000
        rts
.endproc
