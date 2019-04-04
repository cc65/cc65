;
; Ullrich von Bassewitz, 2010-11-03
;
; CC65 library: 16x16 => 32 signed multiplication
;

        .export         _imul16x16r32
        .import         imul16x16r32, popax

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 16x16 => 32 signed multiplication routine.


.proc   _imul16x16r32

        sta     ptr1
        stx     ptr1+1
        jsr     popax
        jmp     imul16x16r32

.endproc

