;
; Ullrich von Bassewitz, 2009-11-02
;
; void __fastcall__ tgi_vectorchar (const unsigned char* Ops);
; /* Draw one character of the vector font at the current graphics cursor
;  * position using the current font magnification.
;  */
;

        .export         _tgi_vectorchar

        .import         imul16x16r32, umul16x16r32, negax, negeax

        .include        "tgi-kernel.inc"
        .include        "zeropage.inc"


;----------------------------------------------------------------------------
; Data

Ops     = regbank
Flag    = regbank+2

.bss
X1:     .res    2
Y1:     .res    2
X2:     .res    2
Y2:     .res    2



;----------------------------------------------------------------------------
;

.code
.proc   _tgi_vectorchar

; Since we will call tgi_lineto, which uses the zero page, and we do also
; need the zero page, make room in the register bank.

        tay
        lda     Ops
        pha
        lda     Ops+1
        pha
        lda     Flag
        pha

; Save the pointer

        sty     Ops
        stx     Ops+1

; Main loop executing vector operations

Loop:   lda     _tgi_textscalew+0
        ldx     _tgi_textscalew+1
        jsr     GetProcessedCoord

; X2 = tgi_curx + XMag * XDelta.

        clc
        adc     _tgi_curx+0
        sta     X2+0
        txa
        adc     _tgi_curx+1
        sta     X2+1

; Process the Y value

        lda     _tgi_textscaleh+0
        ldx     _tgi_textscaleh+1
        jsr     GetProcessedCoord

; Y2 = tgi_cury - YMag * YDelta;
; Y2 = tgi_cury + (~(YMag * YDelta) + 1);

        eor     #$FF
        sec                             ; + 1
        adc     _tgi_cury+0
        sta     Y2+0
        txa
        eor     #$FF
        adc     _tgi_cury+1
        sta     Y2+1

; Draw, then move - or just move

        bit     Flag
        bpl     @Move                   ; Jump if move only

        ldy     #7                      ; Copy start coords into zp
:       lda     X1,y
        sta     ptr1,y
        dey
        bpl     :-

        jsr     tgi_line                ; Call the driver

; Move the start position

@Move:  ldy     #3
:       lda     X2,y
        sta     X1,y
        dey
        bpl     :-

; Loop if not done

        bit     Flag
        bvc     Loop

; Done. Restore zp and return.

@Done:  pla
        sta     Flag
        pla
        sta     Ops+1
        pla
        sta     Ops
        rts

.endproc

;----------------------------------------------------------------------------
; Get and process one coordinate value. The scale factor is passed in a/x

.proc   GetProcessedCoord

; Save scale factor as left operand for multiplication

        sta     ptr1
        stx     ptr1+1

; Load delta value

        ldy     #0
        lda     (Ops),y
        inc     Ops
        bne     :+
        inc     Ops+1

; Move bit 7 into Flag

:       asl     a                       ; Flag into carry
        ror     Flag

; Since we know that the scale factor is always positive, we will remember
; the sign of the coordinate offset, make it positive, do an unsigned mul
; and negate the result if the vector was negative. This is faster than
; relying on the signed multiplication, which will do the same, but for
; both operands.

        sta     tmp1                    ; Remember sign of vector offset
        cmp     #$80                    ; Sign bit into carry
        ror     a                       ; Sign extend the value
        bpl     :+
        eor     #$FF
        clc
        adc     #$01                    ; Negate
:       ldx     #$00                    ; High byte is always zero

; Multiplicate with the scale factor.

        jsr     umul16x16r32            ; Multiplicate

; The result is a 16.8 fixed point value. Round it.

        cmp     #$80                    ; frac(val) >= 0.5?
        txa
        adc     #$00
        tay
        lda     sreg
        adc     #$00
        tax
        tya
        bit     tmp1                    ; Check sign
        bpl     :+
        jmp     negax                   ; Negate result if necessary
:       rts

.endproc

