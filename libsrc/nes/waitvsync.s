;
; Written by Groepaz <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; void waitvsync(void);
;

        .export _waitvsync

        .include "nes.inc"

.proc   _waitvsync

wait:   lda     PPU_STATUS
        bpl     wait
        rts

.endproc
