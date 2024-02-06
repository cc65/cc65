;
; Ullrich von Bassewitz, 2002-12-12
;
; PLOT replacement function for the PETs
;

        .export         PLOT
        .import         plot            ; from cputc.s

        .include        "pet.inc"


.proc   PLOT

        bcs     @L1                     ; Fetch values if carry set
        sty     CURS_X
        stx     CURS_Y
        jsr     plot
@L1:    ldy     CURS_X
        ldx     CURS_Y
        rts

.endproc


