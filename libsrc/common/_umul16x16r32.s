;
; Ullrich von Bassewitz, 2010-11-03
;
; CC65 library: 16x16 => 32 unsigned multiplication
;

        .export         _umul16x16r32
        .import         umul16x16r32, popax

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 16x16 => 32 unsigned multiplication routine.

.proc   _umul16x16r32

        sta     ptr1
        stx     ptr1+1
        jsr     popax
        jmp     umul16x16r32

.endproc


