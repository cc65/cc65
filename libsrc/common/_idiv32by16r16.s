;
; Ullrich von Bassewitz, 2009-11-04
;
; CC65 library: 32by16 => 16 signed division
;

        .export         _idiv32by16r16
        .import         idiv32by16r16, incsp4

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 32by16 division.

.proc   _idiv32by16r16

        pha                     ; Save rhs

; Copy from stack to zeropage. This assumes ptr1 and ptr2 are adjacent.

        ldy     #3
@L1:    lda     (sp),y
        sta     ptr1,y
        dey
        bpl     @L1

        lda     #4
        clc
        adc     sp
        sta     sp
        bcc     @L2
        inc     sp+1

@L2:    pla                     ; Old rhs
        jmp     idiv32by16r16

.endproc

