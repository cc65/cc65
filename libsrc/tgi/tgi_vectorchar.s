;
; Ullrich von Bassewitz, 2009-11-02
;
; void __fastcall__ tgi_vectorchar (const unsigned char* Ops);
; /* Draw one character of the vector font at the current graphics cursor
;  * position using the current font magnification.
;  */
;

        .export         _tgi_vectorchar

        .import         push0ax, tosmuleax

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

; Do the actual operation

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

; Push the scale factor

        jsr     push0ax

; Load delta value

        ldy     #0
        lda     (Ops),y
        inc     Ops
        bne     :+
        inc     Ops+1

; Move bit 7 into Flag

:       asl     a                       ; Flag into carry
        ror     Flag

; Sign extend the value

        ldx     #0
        cmp     #$80                    ; Sign bit into carry
        bcc     :+
        dex
:       ror     a                       ; Sign extend the value

; Multiplicate with the scale factor.

        stx     sreg
        stx     sreg+1
        jsr     tosmuleax               ; Multiplicate

; The result is a 16.8 fixed point value. Round and return it.

        cmp     #$80                    ; Check digits after the dec point
        txa
        adc     #$00
        tay
        lda     sreg
        adc     #$00
        tax
        tya
        rts

.endproc

