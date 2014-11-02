;
; Ullrich von Bassewitz, 2009-10-30
;


        .include        "tgi-kernel.inc"
        .include        "tgi-vectorfont.inc"
        .include        "zeropage.inc"

        .import         _strlen, _toascii
        .import         umul8x16r16

;-----------------------------------------------------------------------------
; Aliases for zero page locations

Width   := ptr1
WTab    := ptr2
Text    := ptr3



;-----------------------------------------------------------------------------
; unsigned __fastcall__ tgi_gettextwidth (const char* s);
; /* Calculate the width of the text in pixels according to the current text
; ** style.
; */
;
; Result is  strlen (s) * tgi_textmagw * tgi_fontsizex
;

.code
.proc   _tgi_gettextwidth

        ldy     _tgi_font
        bne     @L1                     ; Jump if vector font

; Return the width of the string for the bitmap font

        jsr     _strlen
        ldy     _tgi_charwidth+1
        sty     ptr1
        jmp     umul8x16r16

; Return the width of the string for the vector font. To save some code, we
; will add up all the character widths and then multiply by the scale factor.
; Since the output routine will scale each single character, the result may
; be slightly different.

@L1:    sta     Text
        stx     Text+1                  ; Save pointer to string

        lda     _tgi_vectorfont+1
        tax
        ora     _tgi_vectorfont
        beq     @L9                     ; Return zero if no font

        lda     _tgi_vectorfont
        clc
        adc     #<(TGI_VECTORFONT::WIDTHS - TGI_VF_FIRSTCHAR)
        sta     WTab
        txa
        adc     #>(TGI_VECTORFONT::WIDTHS - TGI_VF_FIRSTCHAR)
        sta     WTab+1

        ldy     #0
        sty     Width
        sty     Width+1                 ; Zero the total width

; Sum up the widths of the single characters

@L2:    ldy     #0
        lda     (Text),y                ; Get next char
        beq     @L4                     ; Bail out if end of text reached
        jsr     _toascii                ; Convert to ascii
        tay
        lda     (WTab),y                ; Get width of this char
        clc
        adc     Width
        sta     Width
        bcc     @L3
        inc     Width+1
@L3:    inc     Text
        bne     @L2
        inc     Text+1
        bne     @L2

; We have the total width now, scale and return it

@L4:    lda     _tgi_textscalew
        ldx     _tgi_textscalew+1
        jmp     tgi_imulround

; Exit point if no font installed

@L9:    rts

.endproc



