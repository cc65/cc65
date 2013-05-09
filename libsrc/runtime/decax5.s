;
; Ullrich von Bassewitz, 26.03.2001
;
; CC65 runtime: Decrement ax by 5
;

        .export         decax5

        .macpack        generic

.proc   decax5

        sub     #5
        bcs     @L9
        dex
@L9:    rts

.endproc
