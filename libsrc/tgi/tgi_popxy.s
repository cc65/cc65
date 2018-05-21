;
; Ullrich von Bassewitz, 02.10.2002
;
; Helper function for tgi functions. Pops X/Y from stack into ptr1/ptr2
;

        .include        "tgi-kernel.inc"

        .import         popptr1
        .importzp       ptr1, ptr2

.proc   tgi_popxy

        sta     ptr2            ; Y
        stx     ptr2+1
        jmp     popptr1         ; X

.endproc

