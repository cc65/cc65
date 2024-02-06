;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by 6
;

        .export         incax6
        .import         incaxy

.proc   incax6

        ldy     #6
        jmp     incaxy

.endproc

