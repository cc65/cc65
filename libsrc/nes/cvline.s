;
; Ullrich von Bassewitz, 08.08.1998
;
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);
;

        .export         _cvlinexy, _cvline
        .import         gotoxy, putchar, newline
        .importzp       tmp1

        .include        "nes.inc"

_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_cvline:
        cmp     #0              ; Is the length zero?
        beq     L9              ; Jump if done
        sta     tmp1
L1:     lda     #CH_VLINE       ; Vertical bar
        jsr     putchar         ; Write, no cursor advance
        jsr     newline         ; Advance cursor to next line
        dec     tmp1
        bne     L1
L9:     rts



