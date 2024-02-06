;
; Ullrich von Bassewitz, 02.10.2002
;
; Helper function for tgi functions. Pops X/Y from stack into ptr3/ptr4
;

        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr3, ptr4

.proc   tgi_popxy2

        sta     ptr4            ; Y
        stx     ptr4+1
        jsr     popax
        sta     ptr3            ; X
        stx     ptr3+1
        rts

.endproc


