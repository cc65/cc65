;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

        .export         screensize
        .include        "atari.inc"

.proc   screensize

        ldx     RMARGN
        inx
        ldy     #24
        rts

.endproc


