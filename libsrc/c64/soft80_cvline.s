;
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);
;

        .export         soft80_cvline, soft80_cvlinexy
        .import         popa, _gotoxy, putchar, newline ; CHECK/FIX
        .importzp       tmp1

soft80_cvlinexy:
        pha                     ; Save the length
        jsr     popa            ; Get y
        jsr     _gotoxy         ; Call this one, will pop params
        pla                     ; Restore the length and run into soft80_cvlinexy

soft80_cvline:
        cmp     #0              ; Is the length zero?
        beq     L9              ; Jump if done
        sta     tmp1
L1:     lda     #125            ; Vertical bar
        jsr     putchar         ; Write, no cursor advance
        jsr     newline         ; Advance cursor to next line
        dec     tmp1
        bne     L1
L9:     rts



