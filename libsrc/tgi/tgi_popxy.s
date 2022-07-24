;
; 2002-10-02, Ullrich von Bassewitz
; 2018-05-20, Christian Kruger
;
; Helper function for TGI functions. Pops X/Y from arguments into ptr1/ptr2.
;

        .include        "tgi-kernel.inc"

        .import         popptr1
        .importzp       ptr2

.proc   tgi_popxy

        sta     ptr2            ; Y
        stx     ptr2+1
        jmp     popptr1         ; X

.endproc
