;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by 7
;

        .export         incax7
        .import         incaxy

.proc   incax7

        ldy     #7
        jmp     incaxy

.endproc

