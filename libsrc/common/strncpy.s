;
; Ullrich von Bassewitz, 2003-05-04
;
; char* __fastcall__ strncpy (char* dest, const char* src, unsigned size);
;

        .export         _strncpy
        .import         popax, popptr1
        .importzp       ptr1, ptr2, tmp1, tmp2, tmp3

.proc   _strncpy

        eor     #$FF
        sta     tmp1
        txa
        eor     #$FF
        sta     tmp2            ; Store -size - 1

        jsr     popptr1         ; get src
        jsr     popax           ; get dest
        sta     ptr2
        stx     ptr2+1
        stx     tmp3            ; remember for function return

; Copy src -> dest up to size bytes

        ldx     tmp1            ; Load low byte of ones complement of size
        ldy     #$00
L1:     inx
        bne     L2
        inc     tmp2
        beq     L9

L2:     lda     (ptr1),y        ; Copy one character
        sta     (ptr2),y
        beq     L5              ; Bail out if terminator reached (A = 0)
        iny
        bne     L1
        inc     ptr1+1
        inc     ptr2+1          ; Bump high bytes
        bne     L1              ; Branch always

; Fill the remaining bytes.

L3:     inx                     ; Counter low byte
        beq     L6              ; Branch on overflow
L4:     sta     (ptr2),y        ; Clear one byte
L5:     iny                     ; Bump pointer
        bne     L3
        inc     ptr2+1          ; Bump high byte
        bne     L3              ; Branch always

; Bump the counter high byte

L6:     inc     tmp2
        bne     L4

; Done, return dest

L9:     lda     ptr2            ; Get low byte
        ldx     tmp3            ; Get unchanged high byte
        rts

.endproc



