;
; Ullrich von Bassewitz, 08.08.1998
;
; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);
;

        .export         _chlinexy, _chline

        .import         gotoxy, cputdirect
        .importzp       tmp1

        .include        "pce.inc"

_chlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length

_chline:
        cmp     #0              ; Is the length zero?
        beq     L9              ; Jump if done
        sta     tmp1
L1:     lda     #CH_HLINE       ; Horizontal line, screen code
        jsr     cputdirect      ; Direct output
        dec     tmp1
        bne     L1
L9:     rts
