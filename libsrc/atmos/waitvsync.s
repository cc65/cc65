;
; Written by Stefan Haubenthal <polluks@sdf.org>, requires VSync hack
;
; void waitvsync (void);
;

        .export _waitvsync

        .include "atmos.inc"

.proc   _waitvsync

        lda     #%00010000
wait:   and     VIA::PRA2       ; CB1
        bne     wait
        rts

.endproc
