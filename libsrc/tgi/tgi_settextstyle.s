;
; Ullrich von Bassewitz, 2009-10-30
;


        .include        "zeropage.inc"
        .include        "tgi-kernel.inc"

        .import         umul8x16r24
        .import         popa, popax

        .macpack        cpu

;-----------------------------------------------------------------------------
; void __fastcall__ tgi_settextstyle (unsigned width, unsigned height,
;                                     unsigned char dir, unsigned char font);
; /* Set the style for text output. The scaling factors for width and height
; ** are 8.8 fixed point values. This means that $100 = 1 $200 = 2 etc.
; ** dir is one of the TGI_TEXT_XXX constants. font is one of the TGI_FONT_XXX
; ** constants.
; */
;

.proc   _tgi_settextstyle

        sta     _tgi_font               ; Remember the font to use
        jsr     popa
        sta     _tgi_textdir            ; Remember the direction

; Pop the height and run directly into tgi_textscale

        jsr     popax

.endproc

;-----------------------------------------------------------------------------
; void __fastcall__ tgi_settextscale (unsigned width, unsigned height);
; /* Set the scaling for text output. The scaling factors for width and height
; ** are 8.8 fixed point values. This means that $100 = 1 $200 = 2 etc.
; */

.proc   _tgi_settextscale

; Setup the height

        ldy     _tgi_fontheight         ; Get height of bitmap font in pixels
        sty     ptr1                    ; Save for later
        ldy     #6                      ; Address the height
        jsr     process_onedim          ; Process height

; Setup the width

        jsr     popax                   ; Get width scale into a/x
        ldy     _tgi_fontwidth          ; Get width of bitmap font in pixels
        sty     ptr1                    ; Save for later
        ldy     #0                      ; Address the width

; Process one character dimension. That means:
;
;   - Store the vector font dimension in 8.8 format
;   - If necessary, round up/down to next integer
;   - Store the bitmap font dimension in 8.8 format
;   - Multiply by size of bitmap char in pixels
;   - Store the bitmap font size in 8.8 format
;

process_onedim:

        jsr     store                   ; Store vector font scale factor
        bit     _tgi_flags              ; Fine grained scaling support avail?
        bmi     @L2                     ; Jump if yes

        asl     a                       ; Round to nearest full integer
        bcc     @L1
        inx
@L1:    lda     #0

@L2:    jsr     store                   ; Store bitmap font scale factor

; The size of the font in pixels in the selected dimension is already in ptr1
; So if we call umul8x16r24 we get the size in pixels in 16.8 fixed point.
; Disallowing characters larger than 256 pixels, we just drop the high byte
; and remember the low 16 bit as size in 8.8 format.

.if (.cpu .bitand ::CPU_ISET_65SC02)
        phy                             ; Save Y
        jsr     umul8x16r24
        ply                             ; Restore Y
.else
        sty     tmp1                    ; Save Y
        jsr     umul8x16r24
        ldy     tmp1                    ; Restore Y
.endif

store:  sta     _tgi_textscalew,y
        iny
        pha
        txa
        sta     _tgi_textscalew,y
        iny
        pla
        rts

.endproc


