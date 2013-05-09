;
; Ullrich von Bassewitz, 26.03.2001
;
; CC65 runtime: Decrement ax by 8
;

        .export         decax8

        .macpack        generic

.proc   decax8

        sub     #8
        bcs     @L9
        dex
@L9:    rts

.endproc
