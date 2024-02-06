;
; Ullrich von Bassewitz, 26.03.2001
;
; CC65 runtime: Decrement ax by 4
;

        .export         decax4

        .macpack        generic

.proc   decax4

        sub     #4
        bcs     @L9
        dex
@L9:    rts

.endproc
