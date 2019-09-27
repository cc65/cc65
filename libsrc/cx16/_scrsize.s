;
; 2019-09-16, Greg King
;
; Screen size info
;

        .export         screensize

        .include        "cx16.inc"

screensize:
        ldx     LLEN
        ldy     NLINES
        rts
