;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by 3
;

        .export         incax3
        .import         incaxy

.proc   incax3

        ldy     #3
        jmp     incaxy

.endproc

