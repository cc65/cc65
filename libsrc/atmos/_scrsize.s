;
; 2003-04-13, Ullrich von Bassewitz
; 2013-07-16, Greg King
;
; Screen size variables
;

        .export         screensize
        .include        "atmos.inc"

.proc   screensize

        ldx     #SCREEN_XSIZE
        ldy     #SCREEN_YSIZE
        rts

.endproc


