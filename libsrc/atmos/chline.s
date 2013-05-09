;
; Ullrich von Bassewitz, 08.08.1998
;
; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);
;

        .export         _chlinexy, _chline
        .import         setscrptr
        .import         rvs
        .import         popax
        .importzp       ptr2

        .include        "atmos.inc"


_chlinexy:
        pha                     ; Save the length
        jsr     popax           ; Get X and Y
        sta     CURS_Y          ; Store Y
        stx     CURS_X          ; Store X
        pla                     ; Restore the length and run into _chline

_chline:
        tax                     ; Is the length zero?
        beq     @L9             ; Jump if done
        jsr     setscrptr       ; Set ptr2 to screen, won't use X
        txa                     ; Length into A
        clc
        adc     CURS_X
        sta     CURS_X          ; Correct X position by length
        lda     #'-'            ; Horizontal line screen code
        ora     rvs
@L1:    sta     (ptr2),y        ; Write one char
        iny                     ; Next char
        bne     @L2
        inc     ptr2+1          ; Bump high byte of screen pointer
@L2:    dex
        bne     @L1
@L9:    rts

