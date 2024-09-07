;
; Written by Stefan Haubenthal <polluks@sdf.org>, requires VSync hack
;
; void waitvsync (void);
;

        .export _waitvsync

        .include "atmos.inc"

.proc   _waitvsync

wait:   lda     VIA::PRA2
        and     #%00010000      ; CB1
        bne     wait
        rts

.endproc
