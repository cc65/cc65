;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Decrement ax by value in Y
;

        .export         decaxy
        .importzp       tmp1

        .macpack        generic

.proc   decaxy

        sty     tmp1
        sub     tmp1
        bcs     @L9
        dex
@L9:    rts

.endproc

