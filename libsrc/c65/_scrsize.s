;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

        .export         screensize
        .include        "cbm_kernal.inc"

.proc   screensize

        jsr     SCREEN
        inx
        iny
        rts

.endproc
