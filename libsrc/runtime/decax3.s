;
; Ullrich von Bassewitz, 26.03.2001
;
; CC65 runtime: Decrement ax by 3
;

        .export         decax3

        .macpack        generic

.proc   decax3

        sub     #3
        bcs     @L9
        dex
@L9:    rts

.endproc
