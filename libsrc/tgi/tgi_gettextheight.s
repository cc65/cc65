;
; Ullrich von Bassewitz, 2009-10-30
;


        .include        "tgi-kernel.inc"
        .include        "tgi-vectorfont.inc"
        .include        "zeropage.inc"


;-----------------------------------------------------------------------------
; unsigned __fastcall__ tgi_gettextheight (const char* s);
; /* Calculate the height of the text in pixels according to the current text
; ** style.
; */
;

.proc   _tgi_gettextheight        

        ldy     _tgi_font
        bne     @L2                     ; Jump if vector font

; Return the height for the bitmap font

        lda     _tgi_charheight
        ldx     #0
@L1:    rts

; Return the height for the vector font

@L2:    lda     _tgi_vectorfont
        tax
        ora     _tgi_vectorfont+1
        beq     @L1                     ; Return zero if no font

        stx     ptr1
        lda     _tgi_vectorfont+1
        sta     ptr1+1
        ldy     #TGI_VECTORFONT::HEIGHT
        lda     (ptr1),y                ; Get height of font

        sta     ptr1
        lda     #0
        sta     ptr1+1                  ; Save base height in ptr1

        lda     _tgi_textscaleh
        ldx     _tgi_textscaleh+1       ; Get scale factor ...
        jmp     tgi_imulround           ; ... and return scaled result

.endproc


