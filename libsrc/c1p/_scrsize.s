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

.proc   screensize

        ldx     SCR_LINELEN
        inx                     ; Variable is one less
        ldy     #25
        rts

.endproc

