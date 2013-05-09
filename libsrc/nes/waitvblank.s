;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; void waitvblank(void);
;

        .export _waitvblank

        .include "nes.inc"

.proc   _waitvblank

wait:   lda     PPU_STATUS
        bpl     wait
        rts

.endproc
