;
; Ullrich von Bassewitz, 08.08.1998
;
; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);
;

        .export         _chlinexy, _chline
        .import         popa, _gotoxy, cputdirect, setcursor
        .importzp       tmp1

.ifdef __ATARI5200__
CHRCODE =       14
.else
CHRCODE =       $12+64
.endif

_chlinexy:
        pha                     ; Save the length
        jsr     popa            ; Get y
        jsr     _gotoxy         ; Call this one, will pop params
        pla                     ; Restore the length

_chline:
        cmp     #0              ; Is the length zero?
        beq     L9              ; Jump if done
        sta     tmp1
L1:     lda     #CHRCODE        ; Horizontal line, screen code
        jsr     cputdirect      ; Direct output
        dec     tmp1
        bne     L1
L9:     jmp     setcursor
