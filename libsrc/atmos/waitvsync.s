;
; Written by Stefan Haubenthal <polluks@sdf.org>, requires VSync hack
;
; void waitvsync (void);
;

        .export _waitvsync

        .include "atmos.inc"

.proc   _waitvsync

        lda     #%00010000      ; CB1
wait:   and     VIA::PRA2
        bne     wait
        rts

.endproc
