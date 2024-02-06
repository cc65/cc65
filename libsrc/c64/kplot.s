;
; Ullrich von Bassewitz, 2002-12-09, 2009-09-27
;
; PLOT replacement function for the C64. The kernal function in the -02 kernals
; does not set the pointer to the color RAM correctly, so we need to fix that.
;

        .export         PLOT

.scope  KERNAL
        .include        "cbm_kernal.inc"
.endscope

.proc   PLOT

        bcs     @L1
        jsr     KERNAL::PLOT            ; Set cursor position using original ROM PLOT
        jmp     KERNAL::UPDCRAMPTR      ; Set pointer to color RAM to match new cursor position

@L1:    jmp     KERNAL::PLOT            ; Get cursor position

.endproc
