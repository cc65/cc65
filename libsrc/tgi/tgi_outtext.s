;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_outtext (const char* s);
; /* Output text at the current graphics cursor position. */


        .include        "tgi-kernel.inc"
        .include        "tgi-vectorfont.inc"
        .include        "zeropage.inc"

        .import         _toascii
        .import         popax, negax


;----------------------------------------------------------------------------
; Data

text    := regbank
font    := regbank              ; Same as text
widths  := regbank+2

;----------------------------------------------------------------------------
;

.proc   _tgi_outtext

        ldy     _tgi_font               ; Bit or vectorfont?
        bne     VectorFont

; Handle bitmapped font output

        sta     ptr3
        stx     ptr3+1                  ; Pass s in ptr3 to driver
        pha
        txa
        pha                             ; Save s on stack for later

        jsr     tgi_curtoxy             ; Copy curx/cury into ptr1/ptr2
        jsr     tgi_outtext             ; Call the driver

        pla
        tax
        pla                             ; Restore s
        jsr     _tgi_gettextwidth       ; Get width of text string

; Move the graphics cursor by the amount in a/x

MoveCursor:
        ldy     _tgi_textdir            ; Horizontal or vertical text?
        beq     @L1                     ; Jump if horizontal

; Move graphics cursor for vertical text

        jsr     negax
        ldy     #2                      ; Point to _tgi_cury

; Move graphics cursor for horizontal text

@L1:    clc
        adc     _tgi_curx,y
        sta     _tgi_curx,y
        txa
        adc     _tgi_curx+1,y
        sta     _tgi_curx+1,y
Done:   rts

; Handle vector font output. First, check if we really have a registered
; vector font. Bail out if this is not the case.

VectorFont:
        tay
        lda     _tgi_vectorfont         ; Do we have a vector font?
        ora     _tgi_vectorfont+1
        beq     Done                    ; Bail out if not

; Check if the font in the given size is partially out of the screen. We
; do this in vertical direction here, and in horizontal direction before
; outputting a character.

        ; (todo)

; Save zero page variable on stack and save

        lda     text
        pha
        lda     text+1
        pha
        lda     widths
        pha
        lda     widths+1
        pha

        sty     text
        stx     text+1                  ; Store pointer to string

        lda     _tgi_vectorfont
        clc
        adc     #<(TGI_VECTORFONT::WIDTHS - TGI_VF_FIRSTCHAR)
        sta     widths
        lda     _tgi_vectorfont+1
        adc     #>(TGI_VECTORFONT::WIDTHS - TGI_VF_FIRSTCHAR)
        sta     widths+1

; Output the text string

@L1:    ldy     #0
        lda     (text),y                ; Get next character from string
        beq     EndOfText
        jsr     _toascii                ; Convert to ascii
        pha                             ; Save char in A
        jsr     _tgi_vectorchar         ; Output it
        pla

; Move the graphics cursor by the width of the char

        tay
        lda     (widths),y              ; Get width of this char
        sta     ptr1
        lda     #0
        sta     ptr1+1
        lda     _tgi_textscalew
        ldx     _tgi_textscalew+1       ; Get scale factor
        jsr     tgi_imulround           ; Multiplcate and round
        jsr     MoveCursor              ; Move the graphics cursor

; Next char in string
                                        
        inc     text
        bne     @L1
        inc     text+1
        bne     @L1

; Done. Restore registers and return

EndOfText:
        pla
        sta     widths+1
        pla
        sta     widths
        pla
        sta     text+1
        pla
        sta     text
        rts

.endproc

