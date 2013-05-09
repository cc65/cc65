;
; Ullrich von Bassewitz, 26.03.2001
;
; CC65 runtime: Decrement ax by 7
;

        .export         decax7

        .macpack        generic

.proc   decax7

        sub     #7
        bcs     @L9
        dex
@L9:    rts

.endproc
