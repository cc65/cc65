;
; Ullrich von Bassewitz, 02.10.2002
;
; Helper function for tgi functions. Pops X/Y from stack into ptr1/ptr2
;

        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr1, ptr2

.proc   tgi_popxy

        sta     ptr2            ; Y
        stx     ptr2+1
        jsr     popax
        sta     ptr1            ; X
        stx     ptr1+1
        rts

.endproc

