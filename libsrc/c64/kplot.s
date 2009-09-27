;
; Ullrich von Bassewitz, 2002-12-09, 2009-09-27
;
; PLOT replacement function for the C64. The kernal function in the -02 kernals
; does not set the pointer to the color RAM correctly, so we need to fix that.
;

        .export         PLOT


.proc   PLOT

        bcs     @L1
        jsr     $FFF0                   ; Set cursor position
        jmp     $EA24                   ; Set pointer to color RAM

@L1:    jmp     $FFF0                   ; Get cursor position

.endproc


