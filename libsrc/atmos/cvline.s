;
; Ullrich von Bassewitz, 2003-04-13
;
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);
;

        .export         _cvlinexy, _cvline
        .import         setscrptr
        .import         rvs
        .import         popax
        .importzp       ptr2

        .include        "atmos.inc"


_cvlinexy:
        pha                     ; Save the length
        jsr     popax           ; Get X and Y
        sta     CURS_Y          ; Store Y
        stx     CURS_X          ; Store X
        pla                     ; Restore the length and run into _cvline

_cvline:
        tax                     ; Is the length zero?
        beq     @L9             ; Jump if done
@L1:    jsr     setscrptr       ; Set ptr2 to screen, won't use X
        lda     #'|'
        ora     rvs
        sta     (ptr2),y        ; Write one char
        inc     CURS_Y
@L2:    dex
        bne     @L1
@L9:    rts


