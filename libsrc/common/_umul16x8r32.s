;
; Ullrich von Bassewitz, 2011-07-10
;
; CC65 library: 16x8 => 32 unsigned multiplication
;

        .export         _umul16x8r32
        .import         umul8x16r24, popax

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 16x8 => 32 unsigned multiplication routine. We use 8x16 => 24 and clear
; the high byte of the result

.proc   _umul16x8r32

        sta     ptr1
        lda     #0
        sta     sreg+1                  ; Clear high byte of result
        jsr     popax
        jmp     umul8x16r24

.endproc


