;
; Oliver Schmidt, 2014-03-27
;
; CC65 library: 8x8 => 16 unsigned multiplication
;

        .export         _cc65_umul8x8r16
        .import         umul8x8r16, popa, ptr1:zp


;---------------------------------------------------------------------------
; 8x8 => 16 unsigned multiplication routine.


.proc   _cc65_umul8x8r16

        sta     ptr1
        jsr     popa
        jmp     umul8x8r16

.endproc
