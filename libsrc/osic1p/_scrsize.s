;
; based on PET implementation
;
; originally by:
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

        .export         screensize
        .import         ScrWidth, ScrHeight

        .include        "extzp.inc"
        .include        "osic1p.inc"

.proc   screensize
        ldx     ScrWidth
        ldy     ScrHeight
        rts
.endproc
