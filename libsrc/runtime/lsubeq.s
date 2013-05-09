;                                 
; Ullrich von Bassewitz, 07.04.2000
;
; CC65 runtime: -= operator
;
; On entry, the low byte of the address of the variable to decrement is
; in ptr1, the high byte is in Y, and the decrement is in eax.
;

        .export         lsubeq1, lsubeqa, lsubeq
        .importzp       sreg, ptr1


lsubeq1:
        lda     #$01

lsubeqa:
        ldx     #$00
        stx     sreg
        stx     sreg+1

lsubeq: sty     ptr1+1                  ; Store high byte of address
        ldy     #$00                    ; Address low byte
        sec

        eor     #$FF
        adc     (ptr1),y                ; Subtract byte 0
        sta     (ptr1),y
        pha                             ; Save byte 0 of result for later

        iny                             ; Address byte 1
        txa
        eor     #$FF
        adc     (ptr1),y                ; Subtract byte 1
        sta     (ptr1),y
        tax

        iny                             ; Address byte 2
        lda     (ptr1),y
        sbc     sreg
        sta     (ptr1),y
        sta     sreg

        iny                             ; Address byte 3
        lda     (ptr1),y
        sbc     sreg+1
        sta     (ptr1),y
        sta     sreg+1

        pla                             ; Retrieve byte 0 of result

        rts                             ; Done




