;
; Ullrich von Bassewitz, 2009-11-04
;
; CC65 library: 32by16 => 16 unsigned division
;

        .export         _udiv32by16r16
        .import         udiv32by16r16m, incsp4

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 32by16 division.

.proc   _udiv32by16r16

        sta     ptr3
        stx     ptr3+1                  ; Store rhs

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

@L2:    jmp     udiv32by16r16m

.endproc

