;
; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);
;

        .export         _chlinexy, _chline
 
        .import         rvs, display_conio, update_adscr
        .import         popax

        .include        "telestrat.inc"


_chlinexy:
        pha                     ; Save the length
        jsr     popax           ; Get X and Y
        sta     SCRY            ; Store Y
        stx     SCRX            ; Store X
        jsr     update_adscr
        pla                     ; Restore the length and run into _chline

_chline:
        tax                     ; Is the length zero?
        beq     @L9             ; Jump if done
@L1:        
        lda     #'-'            ; Horizontal line screen code
        ora     rvs

        jsr     display_conio

@L2:    dex
        bne     @L1
@L9:    rts

