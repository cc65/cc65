;
; Ullrich von Bassewitz, 2009-11-02
;
; void __fastcall__ tgi_vectorchar (const unsigned char* Ops);
; /* Draw one character of the vector font at the current graphics cursor
; ** position using the current font magnification.
; */
;

        .import         imul16x16r32, umul16x16r32, negax, negeax

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

;----------------------------------------------------------------------------
; Get the next operation from the Ops pointer, remove the flag bit and sign
; extend the 8 bit value to 16 bits.

.code
.proc   GetOp

; Load delta value

        ldy     #0
        lda     (Ops),y
        inc     Ops
        bne     :+
        inc     Ops+1

; Move bit 7 into Flag, then sign extend the value in A and extend the sign
; into X.

:       asl     a                       ; Flag into carry
        ror     Flag
        ldx     #0
        cmp     #$80                    ; Sign bit into carry
        ror     a                       ; Sign extend the value
        bpl     :+
        dex                             ; Value is negative

; Done

:       rts

.endproc


;----------------------------------------------------------------------------
; Get and process one coordinate value. The scale factor is passed in a/x

.code
GetProcessedYCoord:
        lda     _tgi_textscaleh+0
        ldx     _tgi_textscaleh+1

GetProcessedCoord:

; Save scale factor as left operand for multiplication

        sta     ptr1
        stx     ptr1+1

; Load next operation value.

        jsr     GetOp

; Multiplicate with the scale factor.

        jmp     tgi_imulround           ; Multiplicate, round and scale

;----------------------------------------------------------------------------
; Add the base coordinate with offset in Y to the value in A/X

.code
.proc   AddBaseCoord

        clc
        adc     _tgi_curx+0,y
        pha
        txa
        adc     _tgi_curx+1,y
        tax
        pla
        rts

.endproc

;----------------------------------------------------------------------------
; Subtract the value in a/x from the base coordinate with offset in Y
; This is
;
;   ax = _tgi_cur[xy] - ax
;
; which can be transformed to
;
;   ax = _tgi_cur[xy] + (~ax + 1);


.code
.proc   SubBaseCoord

        eor     #$FF
        sec                             ; + 1
        adc     _tgi_curx+0,y
        pha
        txa
        eor     #$FF
        adc     _tgi_curx+1,y
        tax
        pla
        rts

.endproc

;----------------------------------------------------------------------------
;

.code
.proc   _tgi_vectorchar

; Multiplicate the char value by two and save into Y

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

; Calculate a pointer to the vector ops for the given char (now in Y). We
; definitely expect a font here, that has to be checked by the caller.

        lda     _tgi_vectorfont
        clc
        adc     #<(TGI_VECTORFONT::CHARS - 2*TGI_VF_FIRSTCHAR)
        sta     Ops
        lda     _tgi_vectorfont+1
        adc     #>(TGI_VECTORFONT::CHARS - 2*TGI_VF_FIRSTCHAR)
        sta     Ops+1

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

; Depending on the text direction, the X vector is either applied to X as
;
;   X2 = _tgi_curx + XMag * XDelta
;
; or applied to Y as
;
;   Y2 = _tgi_cury - XMag * XDelta
;
; which can be transformed to
;
;   Y2 = _tgi_cury + (~(XMag * XDelta) + 1);
;
;
; For the Y component we have
;
;   Y2 = _tgi_cury - YMag * YDelta
;
; which can be transformed to
;
;   Y2 = _tgi_cury + (~(YMag * YDelta) + 1);
;
; or applied to X as
;
;   X2 = _tgi_curx - YMag * YDelta
;
; which can be transformed to
;
;   X2 = _tgi_curx + (~(YMag * YDelta) + 1);
;

        ldy     _tgi_textdir    ; Horizontal or vertical text?
        bne     @Vertical       ; Jump if vertical

; Process horizontal text

        ldy     #0
        jsr     AddBaseCoord
        sta     X2
        stx     X2+1

; Get Y vector

        jsr     GetProcessedYCoord

; Apply to Y

        ldy     #2
        jsr     SubBaseCoord
        sta     Y2
        stx     Y2+1
        jmp     @DrawMove

; Process vertical text

@Vertical:
        ldy     #2
        jsr     SubBaseCoord
        sta     Y2
        stx     Y2+1

; Get Y vector

        jsr     GetProcessedYCoord

; Apply to X

        ldy     #0
        jsr     SubBaseCoord
        sta     X2
        stx     X2+1

; Draw, then move - or just move

@DrawMove:
        bit     Flag
        bpl     @Move                   ; Jump if move only

.if     0
        ldy     #7                      ; Copy start coords into zp
:       lda     X1,y
        sta     ptr1,y
        dey
        bpl     :-

        jsr     tgi_line                ; Call the driver
.else
        ldy     #7                      ; Copy start coords
:       lda     X1,y
        sta     tgi_clip_x1,y
        dey
        bpl     :-

        jsr     tgi_clippedline         ; Call line clipper
.endif

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

        pla
        sta     Flag
        pla
        sta     Ops+1
        pla
        sta     Ops
        rts

.endproc

