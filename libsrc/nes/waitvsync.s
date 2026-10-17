;
; Written by Groepaz <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Rewritten by Brandon Woodward
;
; void waitvsync(void);
;

        .export         _waitvsync

        .include        "nes.inc"

.proc   _waitvsync

        ldx     #0
        stx     VBLANK_FLAG
@wait:  lda     VBLANK_FLAG
        beq     @wait
        rts

.endproc
