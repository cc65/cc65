;
; Ullrich von Bassewitz, 2003-04-13
;
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);
;

        .export         _cvlinexy, _cvline

        .import         rvs, display_conio, update_adscr

        .import         popax

        .include        "telestrat.inc"


_cvlinexy:
        pha                     ; Save the length
        jsr     popax           ; Get X and Y
        sta     SCRY            ; Store Y
        stx     SCRX            ; Store X
        jsr     update_adscr
        pla                     ; Restore the length and run into _cvline

_cvline:
        tax                     ; Is the length zero?
        beq     @L9             ; Jump if done
@L1:
        lda     #'|'
        ora     rvs
        ldy     SCRX
        sta     (ADSCR),y
        ; compute next line
        inc     SCRY
        jsr     update_adscr
@L2:    dex
        bne     @L1
@L9:    rts


