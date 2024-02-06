;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by 8
;

        .export         incax8
        .import         incaxy

.proc   incax8

        ldy     #8
        jmp     incaxy

.endproc

