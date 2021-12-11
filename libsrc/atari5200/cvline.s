;
; Ullrich von Bassewitz, 08.08.1998
;
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);
;
        .include "atari5200.inc"
        
        .export         _cvlinexy, _cvline
        .import         gotoxy, putchar
        .importzp       tmp1

_cvlinexy:
        pha                     ; Save the length
        jsr     gotoxy          ; Call this one, will pop params
        pla                     ; Restore the length and run into _cvline

_cvline:
        cmp     #0              ; Is the length zero?
        beq     L9              ; Jump if done
        sta     tmp1
L1:     lda     COLCRS_5200
        pha
        lda     #CH_VLINE       ; Vertical bar
        jsr     putchar         ; Write, no cursor advance
        pla
        sta     COLCRS_5200
        inc     ROWCRS_5200
        dec     tmp1
        bne     L1
L9:     rts
