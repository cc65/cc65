;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_outtext (const char* s);
; /* Output text at the current graphics cursor position. */


        .include        "tgi-kernel.inc"
        .include        "tgi-vectorfont.inc"
        .include        "zeropage.inc"

        .import         popax, negax


;----------------------------------------------------------------------------
; Data

text    := regbank

;----------------------------------------------------------------------------
;

.proc   _tgi_outtext

        ldy     _tgi_font       ; Bit or vectorfont?
        bne     VectorFont

; Handle bitmapped font output

        sta     ptr3
        stx     ptr3+1          ; Pass s in ptr3 to driver
        pha
        txa
        pha                     ; Save s on stack for later

        jsr     tgi_curtoxy     ; Copy curx/cury into ptr1/ptr2
        jsr     tgi_outtext     ; Call the driver

        pla
        tax
        pla                     ; Restore s
        jsr     _tgi_textwidth  ; Get width of text string
        ldy     _tgi_textdir    ; Horizontal or vertical text?
        beq     @L1             ; Jump if horizontal

; Move graphics cursor for vertical text

        jsr     negax
        ldy     #2              ; Point to _tgi_cury

; Move graphics cursor for horizontal text

@L1:    clc
        adc     _tgi_curx,y
        sta     _tgi_curx,y
        txa
        adc     _tgi_curx+1,y
        sta     _tgi_curx+1,y
        rts

; Handle vector font output

VectorFont:
        tay
        lda     _tgi_vectorfont         ; Do we have a vector font?
        ora     _tgi_vectorfont+1
        beq     Done                    ; Bail out if not

        lda     text                    ; Save zero page variable on stack
        pha
        lda     text+1
        pha

        sty     text
        stx     text+1                  ; Store pointer to string

; Output the text string

@L1:    ldy     #0
        lda     (text),y                ; Get next character from string
        beq     EndOfText
        jsr     _tgi_vectorchar         ; Output it
        inc     text
        bne     @L1
        inc     text+1
        bne     @L1

; Done. Restore registers and return

EndOfText:
        pla
        sta     text+1
        pla
        sta     text
Done:   rts

.endproc

