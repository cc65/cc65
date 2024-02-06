;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Decrement ax by 2
;

        .export         decax2

        .macpack        generic

.proc   decax2

        sub     #2
        bcs     @L9
        dex
@L9:    rts

.endproc
