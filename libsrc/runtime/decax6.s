;
; Ullrich von Bassewitz, 26.03.2001
;
; CC65 runtime: Decrement ax by 6
;

        .export         decax6

        .macpack        generic

.proc   decax6

        sub     #6
        bcs     @L9
        dex
@L9:    rts

.endproc
