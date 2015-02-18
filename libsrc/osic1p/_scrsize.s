;
; based on PET implementation
;
; originally by:
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

        .export         screensize

        .include        "extzp.inc"
        .include        "osic1p.inc"

.proc   screensize
        ldx     #(SCR_LINELEN + 1)
        ldy     #(SCR_HEIGHT + 1)
        rts
.endproc
