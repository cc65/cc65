;
; Ullrich von Bassewitz, 07.04.2000
; Christian Krueger, 12-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: += operator
;
; On entry, the low byte of the address of the variable to increment is
; in ptr1, the high byte is in Y, and the increment is in eax.
;

        .export         laddeq1, laddeqa, laddeq
        .importzp       sreg, ptr1, tmp1

        .macpack        cpu

laddeq1:
        lda     #$01

laddeqa:
        ldx     #$00
        stx     sreg
        stx     sreg+1

laddeq: sty     ptr1+1                  ; Store high byte of address
        clc

.if (.cpu .bitand ::CPU_ISET_65SC02)
        adc     (ptr1)
        sta     (ptr1)
        ldy     #$01                    ; Address byte 1
.else
        ldy     #$00                    ; Address low byte
        adc     (ptr1),y
        sta     (ptr1),y
        iny                             ; Address byte 1
.endif
        pha                             ; Save byte 0 of result for later

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



