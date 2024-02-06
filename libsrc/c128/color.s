;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         _textcolor, _bgcolor, _bordercolor
        .import         return0

        .include        "c128.inc"


_textcolor:
        bit     MODE            ; Check 80/40 column mode
        bmi     @L1             ; Jump if 80 columns

; 40 column mode

        ldx     CHARCOLOR       ; Get the old color
        sta     CHARCOLOR       ; Set the new color
        txa                     ; Old color -> A
        ldx     #$00            ; Load high byte
        rts

; 80 column mode

@L1:    tax                     ; Move new color to X
        lda     CHARCOLOR       ; Get old color + attributes
        and     #$F0            ; Keep old attributes
        ora     $CE5C,x         ; Translate VIC color -> VDC color
        ldx     CHARCOLOR       ; Get the old color
        sta     CHARCOLOR       ; Set the new color + old attributes
        txa                     ; Old color -> A
        and     #$0F            ; Mask out attributes
        ldx     #$00            ; Load high byte

; translate vdc->vic colour

vdctovic:
        ldy     #16
@L2:    cmp     $CE5C-1,y
        beq     @L3
        dey
        bne     @L2
@L3:    tya
        rts


_bgcolor:
        bit     MODE
        bmi     @L1

; 40 column mode

        ldx     VIC_BG_COLOR0   ; get old value
        sta     VIC_BG_COLOR0   ; set new value
        txa
        ldx     #$00
        rts

; 80 column mode

@L1:    tax                     ; Move new color to X
        lda     $CE5C,x         ; Translate VIC color -> VDC color
        pha
        ldx     #26
        jsr     $CDDA           ; Read vdc register 26
        jsr     vdctovic
        tay
        pla
        ldx     #26
        jsr     $CDCC           ; Write vdc register 26
        tya
        ldx     #$00
        rts


_bordercolor:
        bit     MODE
        bmi     @L1

; 40 column mode

        ldx     VIC_BORDERCOLOR ; get old value
        sta     VIC_BORDERCOLOR ; set new value
        txa
        ldx     #$00
        rts

; 80 column mode

@L1:    jmp     return0

