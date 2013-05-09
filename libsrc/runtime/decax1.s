;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Decrement ax by 1
;

        .export         decax1

        .macpack        generic

.proc   decax1

        sub     #1
        bcs     @L9
        dex
@L9:    rts

.endproc
