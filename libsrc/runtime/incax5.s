;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by 5
;

        .export         incax5
        .import         incaxy

.proc   incax5

        ldy     #5
        jmp     incaxy

.endproc

