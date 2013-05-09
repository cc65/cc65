;
; Ullrich von Bassewitz, 2003-04-13
;
; void cclearxy (unsigned char x, unsigned char y, unsigned char length);
; void cclear (unsigned char length);
;

        .export         _cclearxy, _cclear
        .import         setscrptr  
        .import         rvs
        .import         popax
        .importzp       ptr2

        .include        "atmos.inc"


_cclearxy:
        pha                     ; Save the length
        jsr     popax           ; Get X and Y
        sta     CURS_Y          ; Store Y
        stx     CURS_X          ; Store X
        pla                     ; Restore the length and run into _cclear

_cclear:
        tax                     ; Is the length zero?
        beq     @L9             ; Jump if done
        jsr     setscrptr       ; Set ptr2 to screen, won't use X
        lda     #' '       
        ora     rvs
@L1:    sta     (ptr2),y        ; Write one char
        iny                     ; Next char
        bne     @L2
        inc     ptr2+1          ; Bump high byte of screen pointer
@L2:    dex
        bne     @L1
@L9:    rts


