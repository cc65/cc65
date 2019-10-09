;
; Ullrich von Bassewitz, 2002-12-09
;
; PLOT replacement function for the VIC20. The kernal function does not set
; the pointer to the color RAM correctly.
;

        .export         PLOT

        .include        "vic20.inc"

.proc   PLOT

        bcs     @L1
        jsr     $FFF0                   ; Set cursor position using original ROM PLOT
        jmp     UPDCRAMPTR              ; Set pointer to color RAM

@L1:    jmp     $FFF0                   ; Get cursor position

.endproc

