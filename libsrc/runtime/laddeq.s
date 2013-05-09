;
; Ullrich von Bassewitz, 07.04.2000
;
; CC65 runtime: += operator
;
; On entry, the low byte of the address of the variable to increment is
; in ptr1, the high byte is in Y, and the increment is in eax.
;

        .export         laddeq1, laddeqa, laddeq
        .importzp       sreg, ptr1, tmp1


laddeq1:
        lda     #$01

laddeqa:
        ldx     #$00
        stx     sreg
        stx     sreg+1

laddeq: sty     ptr1+1                  ; Store high byte of address
        ldy     #$00                    ; Address low byte
        clc

        adc     (ptr1),y
        sta     (ptr1),y
        pha                             ; Save byte 0 of result for later

        iny                             ; Address byte 1
        txa
        adc     (ptr1),y                ; Load byte 1
        sta     (ptr1),y
        tax

        iny                             ; Address byte 2
        lda     sreg
        adc     (ptr1),y
        sta     (ptr1),y
        sta     sreg

        iny                             ; Address byte 3
        lda     sreg+1
        adc     (ptr1),y
        sta     (ptr1),y
        sta     sreg+1

        pla                             ; Retrieve byte 0 of result

        rts                             ; Done



