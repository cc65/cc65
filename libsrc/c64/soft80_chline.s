;
; Groepaz/Hitmen, 12.10.2015
;
; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);
;

        .export         soft80_chlinexy, soft80_chline
        .import         popa, _gotoxy, soft80_cputdirect
        .importzp       tmp1

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_chlinexy:
        pha                             ; Save the length
        jsr     popa                    ; Get y
        jsr     _gotoxy                 ; Call this one, will pop params
        pla                             ; Restore the length

soft80_chline:
        cmp     #0                      ; Is the length zero?
        beq     L9                      ; Jump if done
        sta     tmp1
L1:     lda     #CH_HLINE               ; Horizontal line, petscii code
        jsr     soft80_cputdirect       ; Direct output
        dec     tmp1
        bne     L1
L9:     rts




