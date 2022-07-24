;
; 2014-03-27, Oliver Schmidt
; 2014-05-08, Greg King
;
; CC65 library: 8x8 => 16 signed multiplication
;

        .export         _imul8x8r16
        .import         imul8x8r16, popa, ptr1:zp


;---------------------------------------------------------------------------
; 8x8 => 16 signed multiplication routine.


.proc   _imul8x8r16

        sta     ptr1
        jsr     popa
        jmp     imul8x8r16

.endproc
