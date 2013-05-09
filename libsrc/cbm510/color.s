;
; Ullrich von Bassewitz, 13.09.2001
;

        .export         _textcolor, _bgcolor, _bordercolor
        .import         sys_bank, restore_bank
        .import         vic: zp, CHARCOLOR: zp

        .include        "cbm510.inc"


; ------------------------------------------------------------------------
; unsigned char __fastcall__ textcolor (unsigned char color);
; /* Set the color for text output. The old color setting is returned. */
;

.proc   _textcolor

        ldx     CHARCOLOR       ; get old value
        sta     CHARCOLOR       ; set new value
        txa
        rts

.endproc

; ------------------------------------------------------------------------
; unsigned char __fastcall__ bgcolor (unsigned char color);
; /* Set the color for the background. The old color setting is returned. */
;

.proc   _bgcolor

        jsr     sys_bank                ; Switch to the system bank
        pha                             ; Save new color
        ldy     #VIC_BG_COLOR0
        lda     (vic),y                 ; Get current color...
        tax                             ; ...into X
        pla                             ; Get new color
        sta     (vic),y                 ; Set new color
        txa                             ; Get old color into X
        jmp     restore_bank            ; Restore the old color

.endproc

; ------------------------------------------------------------------------
; unsigned char __fastcall__ bordercolor (unsigned char color);
; /* Set the color for the border. The old color setting is returned. */

.proc   _bordercolor

        jsr     sys_bank                ; Switch to the system bank
        pha                             ; Save new color
        ldy     #VIC_BORDERCOLOR
        lda     (vic),y                 ; Get current color...
        tax                             ; ...into X
        pla                             ; Get new color
        sta     (vic),y                 ; Set new color
        txa                             ; Get old color into X
        jmp     restore_bank            ; Restore the old color

.endproc



