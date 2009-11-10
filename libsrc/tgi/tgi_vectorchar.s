;
; Ullrich von Bassewitz, 2009-11-02
;
; void __fastcall__ tgi_vectorchar (const unsigned char* Ops);
; /* Draw one character of the vector font at the current graphics cursor
;  * position using the current font magnification.
;  */
;

        .import         _toascii, imul16x16r32, umul16x16r32, negax, negeax

        .include        "tgi-kernel.inc"
        .include        "tgi-vectorfont.inc"
        .include        "zeropage.inc"

        .macpack        longbranch

;----------------------------------------------------------------------------
; Data

Ops     = regbank
Flag    = regbank+2

.bss
X1:     .res    2
Y1:     .res    2
X2:     .res    2
Y2:     .res    2
BaseX:  .res    2
BaseY:  .res    2
Char:   .res    1

;----------------------------------------------------------------------------
; Get the next operation from the Ops pointer, remove the flag bit and sign
; extend the 8 bit value. On return, the flags are set for the value in A.

.code
.proc   GetOp

; Load delta value

        ldy     #0
        lda     (Ops),y
        inc     Ops
        bne     :+
        inc     Ops+1

; Move bit 7 into Flag, then sign extend the value in A

:       asl     a                       ; Flag into carry
        ror     Flag
        cmp     #$80                    ; Sign bit into carry
        ror     a                       ; Sign extend the value

; Done

        rts

.endproc


;----------------------------------------------------------------------------
; Round a 16.8 fixed point value in eax

.code
.proc   RoundFix

        cmp     #$80                    ; frac(val) >= 0.5?
        txa
        ldx     sreg
        adc     #$00
        bcc     @L1
        inx
@L1:    rts

.endproc

;----------------------------------------------------------------------------
; Get and process one coordinate value. The scale factor is passed in a/x

.code
.proc   GetProcessedCoord

; Save scale factor as left operand for multiplication

        sta     ptr1
        stx     ptr1+1

; Load next operation value. This will set the flags for the value in A.

        jsr     GetOp

; Since we know that the scale factor is always positive, we will remember
; the sign of the coordinate offset, make it positive, do an unsigned mul
; and negate the result if the vector was negative. This is faster than
; relying on the signed multiplication, which will do the same, but for
; both operands.

        sta     tmp1                    ; Remember sign of vector offset
        bpl     :+
        eor     #$FF
        clc
        adc     #$01                    ; Negate
:       ldx     #$00                    ; High byte is always zero

; Multiplicate with the scale factor.

        jsr     umul16x16r32            ; Multiplicate

; The result is a 16.8 fixed point value. Round it.

        jsr     RoundFix

; Check the sign and negate if necessary

        bit     tmp1                    ; Check sign
        bpl     :+
        jmp     negax                   ; Negate result if necessary
:       rts

.endproc

;----------------------------------------------------------------------------
;

.code
.proc   _tgi_vectorchar

; Convert the character to ASCII, multiplicate by two and save into Y

        jsr     _toascii
        asl     a
        tay

; Since we will call tgi_lineto, which uses the zero page, and we do also
; need the zero page, make room in the register bank.

        lda     Ops
        pha
        lda     Ops+1
        pha
        lda     Flag
        pha

; Get the width of the char in question

        lda     _tgi_vectorfont
        clc
        adc     #<(TGI_VECTORFONT::WIDTHS - TGI_VF_FIRSTCHAR)
        sta     Ops
        lda     _tgi_vectorfont+1
        adc     #>(TGI_VECTORFONT::WIDTHS - TGI_VF_FIRSTCHAR)
        sta     Ops+1
        lda     (Ops),y

; Save the character

        sty     Char

; Calculate the width of the character by multiplying with the scale
; factor for the width

        sta     ptr1
        lda     #0
        sta     ptr1+1

        lda     _tgi_textscalew
        ldx     _tgi_textscalew+1
        jsr     umul16x16r32
        jsr     RoundFix

; Store the current value of the graphics cursor into BaseX/BaseY, then
; move it to the next character position

        pha
        ldy     #3
:       lda     _tgi_curx,y
        sta     BaseX,y
        dey
        bpl     :-
        pla

        ldy     _tgi_textdir
        beq     :+                      ; Jump if horizontal text

        jsr     negax
        ldy     #2                      ; Offset of tgi_cury

; Advance graphics cursor

:       clc
        adc     _tgi_curx,y
        sta     _tgi_curx,y
        txa
        adc     _tgi_curx+1,y
        sta     _tgi_curx+1,y

; Calculate a pointer to the vector ops for the given char (now in Y). We
; definitely expect a font here, that has to be checked by the caller.

        lda     _tgi_vectorfont
        clc
        adc     #<(TGI_VECTORFONT::CHARS - 2*TGI_VF_FIRSTCHAR)
        sta     Ops
        lda     _tgi_vectorfont+1
        adc     #>(TGI_VECTORFONT::CHARS - 2*TGI_VF_FIRSTCHAR)
        sta     Ops+1

        ldy     Char
        iny
        lda     (Ops),y
        tax
        dey
        lda     (Ops),y
        sta     Ops
        stx     Ops+1

; Main loop executing vector operations

Loop:   lda     _tgi_textscalew+0
        ldx     _tgi_textscalew+1
        jsr     GetProcessedCoord       ; Get X vector

; X2 = BaseX + XMag * XDelta.

        clc
        adc     BaseX+0
        sta     X2+0
        txa
        adc     BaseX+1
        sta     X2+1

; Process the Y value

        lda     _tgi_textscaleh+0
        ldx     _tgi_textscaleh+1
        jsr     GetProcessedCoord

; Y2 = BaseY - YMag * YDelta;
; Y2 = BaseY + (~(YMag * YDelta) + 1);

        eor     #$FF
        sec                             ; + 1
        adc     BaseY+0
        sta     Y2+0
        txa
        eor     #$FF
        adc     BaseY+1
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

Done:   pla
        sta     Flag
        pla
        sta     Ops+1
        pla
        sta     Ops
        rts

.endproc

