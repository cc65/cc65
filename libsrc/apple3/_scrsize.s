;
; Ullrich von Bassewitz, 26.10.2000
; Robert Justice, 2026
;
; Screen size variables
;

        .export         screensize
        .import         consvpwidth, consvpheight

        .include        "apple3.inc"

screensize:
        ldx     consvpwidth
        ldy     consvpheight
        rts
