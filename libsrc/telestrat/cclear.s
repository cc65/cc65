;
; 2019-07-07, Jede (jede@oric.org)
;
; void cclearxy (unsigned char x, unsigned char y, unsigned char length);
; void cclear (unsigned char length);
;

        .export         _cclearxy, _cclear
        .import         update_adscr

        .importzp       tmp1 
        .import         popax
        .include        "telestrat.inc"


_cclearxy:
        pha                     ; Save the length
        jsr     popax           ; Get X and Y
        sta     SCRY            ; Store Y
        stx     SCRX            ; Store X
        jsr     update_adscr
        pla                     ; Restore the length and run into _cclear

_cclear:
        tax                     ; Is the length equal to zero?
        beq     @L2             ; Yes we skip
@L1:
        stx     tmp1            ; Save X
        lda     #' '            ; Erase current char
        BRK_TELEMON     XFWR
        ldx     tmp1
        dex
        bne     @L1
@L2:
        rts
